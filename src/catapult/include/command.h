#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <list>
#include <memory>
#include <stack>
#include <variant>

namespace catapult {

template<class Receiver>
class Command {
  public:
    virtual ~Command () = default;
    virtual void execute (Receiver &receiver) const = 0;
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

template<class Receiver>
class Undoable {
  public:
    virtual ~Undoable () = default;
    virtual void undo (Receiver &receiver) const = 0;
};

template<class Receiver>
class UndoableCommand: public Command<Receiver>, public Undoable<Receiver> {
};

template<class Receiver>
class ReverseCommand: public Command<Receiver> {
  public:
    ReverseCommand (Undoable<Receiver> *command): _command (command) {};
    ReverseCommand (const ReverseCommand &) = delete;
    ReverseCommand &operator= (ReverseCommand &) = delete;
    virtual ~ReverseCommand () = default;

    void execute (Receiver &receiver) const override { _command->undo (receiver); };

  private:
    std::shared_ptr<Undoable<Receiver>> _command;
};

template<class ...Interfaces>
class Transaction {
  public:
    template<class Interface>
    Transaction& addCommand(Interface&, UndoableCommand<Interface>);
};

}

#endif
