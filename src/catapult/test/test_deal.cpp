#include "deal.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "catalogue.h"
#include "mock/mock_observer.h"

namespace catapult::testing
{

using namespace ::testing;

TEST(TestMultiplePurchaseDiscount, TestValueCalculation) {
  Product mockProduct { "MockProduct", 123 };
  MultiplePurchaseDiscount underTest { mockProduct, 3 };
  EXPECT_EQ(underTest.getValue(), 369);
}

template<class State>
class TestObservable: public BaseObservable<State> {
  public:
    using BaseObservable<State>::_notifyObservers;
};

TEST(TestMultiplePurchaseDeal, TestDiscountsAreCalculated) {
  TestObservable<StockChangeMessage> testObservable;
  Product mockProduct { "MockProduct", 123 };
  MultiplePurchaseDeal underTest { {mockProduct} };
  testObservable.registerObserver(underTest);

  StockChangeMessage stockChangeMessage { mockProduct, 6 };
  testObservable._notifyObservers(stockChangeMessage);
  auto discounts = underTest.getDiscounts();
  EXPECT_EQ(discounts.front()->getValue(), 246);
}

} // namespace catapult::testing

