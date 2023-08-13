#ifndef __MOCK_OBSERVER_H__
#define __MOCK_OBSERVER_H__

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "observer.h"

namespace catapult::testing {

using namespace ::testing;

template<class State>
class MockObserver: public Observer<State> {
  public:
    MOCK_METHOD ((void), notify, (State), (override));
};

}  // namespace catapult::testing

#endif  // __MOCK_OBSERVER_H__
