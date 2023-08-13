#include "mock/mock_observer.h"

#include "observer.h"

namespace catapult::testing
{
using namespace ::testing;

template<class State>
class BaseObservableTest: public BaseObservable<State> {
  FRIEND_TEST(TestBaseObservable, TestNotification);
};

TEST(TestBaseObservable, TestNotification) {
  MockObserver<uint32_t> mockObserver;
  BaseObservableTest<uint32_t> underTest;
  underTest.registerObserver(mockObserver);
  EXPECT_CALL(mockObserver, notify(16));
  underTest._notifyObservers(16);
}

} // namespace catapult::testing
