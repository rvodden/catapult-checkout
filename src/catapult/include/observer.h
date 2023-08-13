#ifndef __OBSERVER_H__
#define __OBSERVER_H__

#include <algorithm>

template<class State>
class Observer {
  public:
    virtual ~Observer () = default;
    virtual void notify (State state) = 0;
};

template<class State>
class Observable {
  public:
    virtual ~Observable () = default;
    virtual void registerObserver (Observer<State> &observer) = 0;

  protected:
    virtual void _notifyObservers (State state) const = 0;
};

template<class State>
class BaseObservable: public Observable<State> {
  public:
    void registerObserver (Observer<State> &observer) override {
      _observers.push_back (std::ref<Observer<State>> (observer));
    };

  protected:
    void _notifyObservers (State state) const override {
      std::ranges::for_each (_observers, [&] (auto &obs) { obs.get ().notify (state); });
    };

  private:
    std::vector<std::reference_wrapper<Observer<State>>> _observers;
};

#endif  // __OBSERVER_H__
