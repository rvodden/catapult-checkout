#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <list>
#include <memory>
#include <variant>

template<class Receiver>
class Executable {
  public:
    virtual ~Executable() = default;
    virtual void execute (Receiver &receiver) const = 0;
};

template <class Interfaces>
using CommandPointer = std::shared_ptr<Executable<Interfaces>>;

template <class ...Interfaces>
using CommandWrapper = std::variant<CommandPointer<Interfaces>...>;

template<class ...Interfaces>
using CommandList = std::list<CommandWrapper<Interfaces...>>;

template<class Interface>
class Receiver: virtual public Interface {
  public:
    virtual ~Receiver() = default;
    void applyCommand(const CommandPointer<Interface> &command) {
      command->execute(*this);
    }
};


//! @brief Inheriting from the MultiInterface template allows class to receive commands from more than one interface.
template<class Interface, class ...Interfaces>
class MultiReceiver: public Receiver<Interface>, public Receiver<Interfaces>... {
  public:
    using Receiver<Interface>::applyCommand;
    using Receiver<Interfaces>::applyCommand...;

    //! @brief dispatch commands to the appropriate `applyCommand` for each interface.
    template<class SubInterface, class ...SubInterfaces>
    void applyCommandList (const CommandList<SubInterface, SubInterfaces...>& commandList) {
      for (const auto &command: commandList) {
        std::visit([=,this](auto& command){ this->applyCommand(command); }, command);
      };
    };
};

template<class Receiver>
class Undoable {
  public:
    virtual ~Undoable () = default;
    virtual void undo (Receiver &receiver) const = 0;
};

template<class Receiver>
class ReverseCommand: public Executable<Receiver> {
  public:
    ReverseCommand (Undoable<Receiver> *command): _command (command) {};
    ReverseCommand (const ReverseCommand &) = delete;
    ReverseCommand &operator= (ReverseCommand &) = delete;
    virtual ~ReverseCommand () = default;

    void execute (Receiver &receiver) const override { _command->undo (receiver); };

  private:
    std::shared_ptr<Undoable<Receiver>> _command;
};

#endif
