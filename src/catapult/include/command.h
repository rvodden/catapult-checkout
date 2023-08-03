#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <list>
#include <memory>

class Command {
  public:
    virtual ~Command () = default;
};

template<class Receiver>
class Executable: public Command {
  public:
    virtual void execute (Receiver &receiver) const = 0;
};

template<class Receiver>
using CommandList = std::list<std::shared_ptr<Executable<Receiver>>>;

template<class SubClass>
class Receiver {
  public:
    virtual ~Receiver () = default;
    void applyCommandList (const CommandList<SubClass>& commandList) {
      for (const auto &command: commandList) {
        command->execute (static_cast<SubClass&>(*this));
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
