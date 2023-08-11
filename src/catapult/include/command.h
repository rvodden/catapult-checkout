#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <list>
#include <memory>
#include <stack>
#include <variant>

#include "exception.h"

namespace catapult {

class UnboundCommandException: public CatapultException {
  public:
    UnboundCommandException (): CatapultException ("The command is not bound to an interface implementation") {};
    // TODO: link to command which broke.
};

template<class Interface>
class Command {
  public:
    virtual ~Command () = default;
    void execute (Interface &interface) { _execute (interface); };

  protected:
    virtual void _execute (Interface &receiver) const = 0;
};

class EmptyInterface {};

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


template<>
class BindableCommand<EmptyInterface>: public Command<EmptyInterface> {
  public:
    ~BindableCommand () override { delete _interface; };

    using Command<EmptyInterface>::execute;
    void execute () { execute (*_getTarget ()); };

    //! @brief associates the command with an interface instance, but does not execute it.
    void bind ([[maybe_unused]] EmptyInterface &interface) {};
    bool isBound () { return true; }

  protected:
    EmptyInterface *_getTarget () const { return _interface; }
    void _execute ([[maybe_unused]] EmptyInterface &interface) const override { _execute (); };
    virtual void _execute () const = 0;
    EmptyInterface *_interface { new EmptyInterface () };
};


template<class Interfaces>
using CommandPointer = std::shared_ptr<Command<Interfaces>>;

template<class... Interfaces>
using CommandWrapper = std::variant<CommandPointer<Interfaces>...>;

//! @brief a std::list of commands each of which is targeted at one of `Interfaces`
template<class... Interfaces>
using CommandList = std::list<CommandWrapper<Interfaces...>>;

template<class Interface>
class Undoable {
  public:
    virtual ~Undoable () = default;
    virtual void undo (Interface &receiver) const = 0;
};

template<class Interface>
class UndoableCommand: public Command<Interface>, public Undoable<Interface> {};

template<class Interfaces>
using UndoableCommandPointer = std::shared_ptr<UndoableCommand<Interfaces>>;

template<class... Interfaces>
using UndoableCommandWrapper = std::variant<UndoableCommandPointer<Interfaces>...>;

//! @brief a std::list of commands each of which is targeted at one of `Interfaces`
template<class... Interfaces>
using UndoableCommandList = std::list<UndoableCommandWrapper<Interfaces...>>;

template<class Interface = EmptyInterface>
class UndoableBindableCommand: public BindableCommand<Interface>, public Undoable<Interface> {
  public:
    using Undoable<Interface>::undo;
    void undo () const { undo (*BindableCommand<Interface>::_getTarget ()); }
};

template<>
class UndoableBindableCommand<EmptyInterface>: public BindableCommand<EmptyInterface>, public Undoable<EmptyInterface> {
  public:
    using Undoable<EmptyInterface>::undo;
    void undo () const { undo (*BindableCommand<EmptyInterface>::_getTarget ()); }
};

template<class Interfaces>
using UndoableBindableCommandPointer = std::shared_ptr<UndoableBindableCommand<Interfaces>>;

template<class... Interfaces>
using UndoableBindableCommandWrapper = std::variant<UndoableBindableCommandPointer<Interfaces>...>;

//! @brief a std::list of commands each of which is targeted at one of `Interfaces`
template<class... Interfaces>
using UndoableBindableCommandList = std::list<UndoableBindableCommandWrapper<Interfaces...>>;


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

template<class... Interfaces>
class Transaction: public BindableCommand<> {
  public:
    ~Transaction () override {
      while (!_beenRun.empty ()) {
        std::visit ([] (const auto &command) { command->undo (); }, _beenRun.top ());
        _beenRun.pop ();
      }
    }

    template<class Interface>
    Transaction<Interfaces...> &then (UndoableBindableCommandPointer<Interface> command) {
      if (!command->isBound ()) {
        throw UnboundCommandException ();
      }
      _toRun.push (command);
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
    mutable std::stack<UndoableBindableCommandWrapper<Interfaces...>> _toRun;
    mutable std::stack<UndoableBindableCommandWrapper<Interfaces...>> _beenRun;
};

template<class... Interfaces>
void Transaction<Interfaces...>::_execute () const {
  while (!_toRun.empty ()) {
    auto command = _toRun.top ();
    std::visit ([] (const auto &command) { command->execute (); }, command);
    _beenRun.push (command);
    _toRun.pop ();
  }
};

}

#endif
