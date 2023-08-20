#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <list>
#include <memory>
#include <stack>
#include <queue>
#include <variant>

#include "exception.h"

namespace catapult {

//! @brief thrown if a bindable command is executed, or added to a collection, before it is bound.
class UnboundCommandException: public CatapultException {
  public:
    UnboundCommandException (): CatapultException ("The command is not bound to an interface implementation") {};
    // TODO: link to command which broke.
};

//! @brief encapsulates actions against an interface so they can be triggered by agents which may not have access to the
//! interface
template<class Interface>
class Command {
  public:
    virtual ~Command () = default;
    void execute (Interface &interface) { _execute (interface); };

  protected:
    virtual void _execute (Interface &receiver) const = 0;
};

//! @brief an interface which requires no methods.
class EmptyInterface {};

//! @brief a command which can be bound to an implementation of the required interface before it is executed
template<class Interface = EmptyInterface>
class BindableCommand: public Command<Interface> {
  public:
    using Command<Interface>::execute;
    void execute () { execute (*_getTarget ()); };

    //! @brief associates the command with an interface instance, but does not execute it.
    void bind (Interface &interface) { _interface = &interface; }
    bool isBound () { return _interface != nullptr; }

  protected:
    Interface *_getTarget () const {
      if (_interface == nullptr) {
        throw UnboundCommandException ();
      }
      return _interface;
    }

  private:
    Interface *_interface { nullptr };
};

//! @brief a specialisation of BindableCommand which does not consume an interface - used by meta commands such as
//! `Transaction`
template<>
class BindableCommand<EmptyInterface>: public Command<EmptyInterface> {
  public:
    BindableCommand() = default;
    BindableCommand(const BindableCommand& other) = default;

    BindableCommand(BindableCommand&& other) noexcept : _interface { other._interface } {
      other._interface = new EmptyInterface();
    };

    ~BindableCommand () override { delete _interface; };

    BindableCommand& operator=(const BindableCommand& other){
      _interface = other._interface;
      return *this;
    };

    BindableCommand& operator=(BindableCommand&& other) noexcept {
      delete _interface;
      _interface = other._interface;
      other._interface = new EmptyInterface();
      return *this;
    };

    using Command<EmptyInterface>::execute;
    void execute () { execute (*_getTarget ()); };

    //! @brief associates the command with an interface instance, but does not execute it.
    void bind ([[maybe_unused]] EmptyInterface &interface) {};
    static bool isBound () { return true; }

  protected:
    EmptyInterface *_getTarget () const { return _interface; }
    void _execute ([[maybe_unused]] EmptyInterface &interface) const override { _execute (); };
    virtual void _execute () const = 0;

  private:
    EmptyInterface *_interface { new EmptyInterface () };
};

//! @brief a pointer to a command
template<class Interfaces>
using CommandPointer = std::shared_ptr<Command<Interfaces>>;

//! @brief an encapsulation of a command which is target at one of, potentially, many Interfaces
template<class... Interfaces>
using CommandWrapper = std::variant<CommandPointer<Interfaces>...>;

//! @brief a std::list of commands each of which is targeted at one of Interfaces
template<class... Interfaces>
using CommandList = std::list<CommandWrapper<Interfaces...>>;


//! @brief encapsulation of reversing an action
template<class Interface>
class Undoable {
  public:
    virtual ~Undoable () = default;
    virtual void undo (Interface &receiver) const = 0;
};

//! @brief a command whos action can be reversed
template<class Interface>
class UndoableCommand: public Command<Interface>, public Undoable<Interface> {};

//! @brief a pointer to an UndoableCommand
template<class Interfaces>
using UndoableCommandPointer = std::shared_ptr<UndoableCommand<Interfaces>>;

//! @brief encapsulation of an UndoableCommand which may be targeted at one of many Interfaces.
template<class... Interfaces>
using UndoableCommandWrapper = std::variant<UndoableCommandPointer<Interfaces>...>;

//! @brief a std::list of commands each of which is targeted at one of many Interfaces.
template<class... Interfaces>
using UndoableCommandList = std::list<UndoableCommandWrapper<Interfaces...>>;

//! @brief a command which can both be bound and reversed.
template<class Interface = EmptyInterface>
class UndoableBindableCommand: public BindableCommand<Interface>, public Undoable<Interface> {
  public:
    using Undoable<Interface>::undo;
    void undo () const { undo (*BindableCommand<Interface>::_getTarget ()); }
};

//! @brief a specialisation of UndoablBindableCommand which does not target any interface.
template<>
class UndoableBindableCommand<EmptyInterface>: public BindableCommand<EmptyInterface>, public Undoable<EmptyInterface> {
  public:
    using Undoable<EmptyInterface>::undo;
    void undo () const { undo (*BindableCommand<EmptyInterface>::_getTarget ()); }
};

//! @brief a pointer to an UndoableBindableCommand
template<class Interfaces>
using UndoableBindableCommandPointer = std::shared_ptr<UndoableBindableCommand<Interfaces>>;

//! @brief an encapsulation of UndoableBindableCommands which do not target and interface.
template<class... Interfaces>
using UndoableBindableCommandWrapper = std::variant<UndoableBindableCommandPointer<Interfaces>...>;

//! @brief a std::list of commands each of which is targeted at one of `Interfaces`
template<class... Interfaces>
using UndoableBindableCommandList = std::list<UndoableBindableCommandWrapper<Interfaces...>>;

//! @brief provides the abiity for a command to be applied to an Interface.
template<class Interface>
class Receiver: virtual public Interface {
  public:
    void applyCommand (const CommandPointer<Interface> &command) { command->execute (*this); }
};

//! @brief Inheriting from the MultiInterface template allows class to receive commands from more than one interface.
template<class Interface, class... Interfaces>
class MultiReceiver: public Receiver<Interface>, public Receiver<Interfaces>... {
  public:
    using Receiver<Interface>::applyCommand;
    using Receiver<Interfaces>::applyCommand...;

    //! @brief dispatch commands to the appropriate `applyCommand` for each interface.
    template<class SubInterface, class... SubInterfaces>
    void applyCommandList (const CommandList<SubInterface, SubInterfaces...> &commandList) {
      for (const auto &command: commandList) {
        std::visit ([=, this] (auto &command) { this->applyCommand (command); }, command);
      };
    }
};

//! @brief generates a command from the reverse action of an UndoableCommand
template<class Interface>
class ReverseCommand: public Command<Interface> {
  public:
    ReverseCommand (Undoable<Interface> *command): _command (command) {};
    ReverseCommand (const ReverseCommand &) = delete;
    ReverseCommand &operator= (ReverseCommand &) = delete;
    virtual ~ReverseCommand () = default;

    void _execute (Interface &receiver) const override { _command->undo (receiver); };

  private:
    std::shared_ptr<Undoable<Interface>> _command;
};

//! @brief encapsulats a set of UndoableBindableCommands which can then only all succeed or all fail
template<class... Interfaces>
class Transaction: public BindableCommand<> {
  public:
    Transaction() = default;

    // Copy must empty _beenRun, as otherwise commands may
    // be rolled back twice.
    Transaction(const Transaction& other): _commands { other._commands }, _beenRun { } {
    }

    Transaction& operator=(const Transaction& other) {
      _commands = other._commands;
      _beenRun = {};
      return *this;
    }

    // Move must empty _beenRun on other, otherwise
    // commands may be rolled back twice
    Transaction(Transaction&& other) noexcept : _commands { other._commands }, _beenRun { other._beenRun } {
    }

    Transaction& operator=(Transaction&& other) noexcept {
      _commands = other._commands;
      _beenRun = other._beenRun;
      other._commands = {};
      other._beenRun = {};
      return *this;
    }


    ~Transaction () override {
      if(!_committed) {
        while (!_beenRun.empty ()) {
          std::visit ([] (const auto &command) { command->undo (); }, _beenRun.top ());
          _beenRun.pop ();
        }
      }
    }

    template<class Interface>
    Transaction<Interfaces...> &then (UndoableBindableCommandPointer<Interface> command) {
      if (!command->isBound ()) {
        throw UnboundCommandException ();
      }
      _commands.push_back (command);
      return *this;
    }

    template<class Interface>
    Transaction<Interfaces...> &then (Interface &interface, UndoableBindableCommandPointer<Interface> command) {
      command->bind (interface);
      return then (command);
    }

  protected:
    void _execute () const override;

  private:
    mutable std::vector<UndoableBindableCommandWrapper<Interfaces...>> _commands;
    mutable std::stack<UndoableBindableCommandWrapper<Interfaces...>> _beenRun;
    mutable bool _committed { false };
};

template<class... Interfaces>
void Transaction<Interfaces...>::_execute () const {
  for ( auto& command : _commands ) {
    std::visit ([] (const auto &command) { command->execute (); }, command);
    _beenRun.push (command);
  }
  _committed = true;
};

}

#endif
