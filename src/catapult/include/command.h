#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <experimental/memory>
#include <list>
#include <memory>
#include <stack>
#include <variant>

#include "exception.h"

namespace catapult {

template<class Interface>
class Command {
  public:
    virtual ~Command () = default;
    void execute (Interface &interface) {
      _interface = &interface;
      _execute (interface);
    };

    //! @brief associates the command with an interface instance, but does not execute it.
    void bind (Interface &interface) { _interface = &interface; }
    bool isBound () { return _interface != nullptr; }

  protected:
    //! @brief returns a pointer to
    Interface* getTarget() {
      if(_interface == nullptr) throw UnboundCommandException(*this);
      return _interface;
    }

  private:
    virtual void _execute (Interface &receiver) const = 0;
    Interface *_interface { nullptr };
};

template<class Interfaces>
using CommandPointer = std::shared_ptr<Command<Interfaces>>;

template<class... Interfaces>
using CommandWrapper = std::variant<CommandPointer<Interfaces>...>;

//! @brief a std::list of commands each of which is targeted at one of `Interfaces`
template<class... Interfaces>
using CommandList = std::list<CommandWrapper<Interfaces...>>;

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
class Undoable {
  public:
    virtual ~Undoable () = default;
    virtual void undo (Interface &receiver) const = 0;
};

template<class Interface>
class UndoableCommand: public Command<Interface>, public Undoable<Interface> {
  public:
    using Undoable<Interface>::undo;
    void undo () const {
      undo(*Command<Interface>::getTarget());
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

template<class Interface>
class UnboundCommandException: public CatapultException {
  public:
    UnboundCommandException (const Command<Interface> &command):
      CatapultException ("The command is not bound to an interface implementation"), _command { command } {};

  private:
    Command<Interface> _command;
};

template<class... Interfaces>
class Transaction {
  public:
    template<class Interface>
    Transaction<Interfaces...> &then (UndoableCommand<Interface> command) {
      if (!command.isBound ()) {
        throw UnboundCommandException (command);
      }
      _stack.push (command);
    }

    template<class Interface>
    Transaction<Interfaces...> &then (Interface &interface, UndoableCommand<Interface> command) {
      command.bind (interface);
      then (command);
    }

    void execute ();

  private:
    std::stack<CommandWrapper<Interfaces...>> _stack;
};

template<class... Interfaces>
void Transaction<Interfaces...>::execute () {

};

}

#endif
