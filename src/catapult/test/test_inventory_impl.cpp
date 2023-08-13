#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mock/mock_observer.h"
#include "inventory_impl.h"

namespace catapult::testing {

using namespace ::testing;

class InventoryTest: public InventoryImpl {
    FRIEND_TEST (TestInventoryImpl, TestAddItemsAndGetQuantity);
    FRIEND_TEST (TestInventoryImpl, TestAddItemsNotifiesObservers);
    FRIEND_TEST (TestInventoryImpl, TestRemoveItems);
    FRIEND_TEST (TestInventoryImpl, TestRemoveItemsNotifiesObservers);
    FRIEND_TEST (TestInventoryImpl, TestRemoveItemsThrowsWhenItemIsOutOfStock);
};

TEST (TestInventoryImpl, TestAddItemsAndGetQuantity) {
  InventoryTest underTest;
  Product mockProduct ("MockProduct", 123);
  underTest._addItems (mockProduct, 234);
  EXPECT_EQ (234, underTest.getQuantity (mockProduct));
}

TEST (TestInventoryImpl, TestAddItemsNotifiesObservers) {
  InventoryTest underTest;
  MockObserver<StockChangeMessage> mockObserver;
  underTest.registerObserver(mockObserver);
  Product mockProduct ("MockProduct", 123);
  EXPECT_CALL(mockObserver, notify);
  underTest._addItems (mockProduct, 234);
  EXPECT_EQ (234, underTest.getQuantity (mockProduct));
}

TEST (TestInventoryImpl, TestGetQuantityWhenOutOfStock) {
  InventoryImpl underTest;
  Product mockProduct ("MockProduct", 123);
  EXPECT_EQ (0, underTest.getQuantity (mockProduct));
}

TEST (TestInventoryImpl, TestRemoveItems) {
  InventoryTest underTest;
  Product mockProduct ("MockProduct", 123);
  underTest._addItems (mockProduct, 234);
  underTest._removeItems (mockProduct, 123);
  EXPECT_EQ (111, underTest.getQuantity (mockProduct));
}

TEST (TestInventoryImpl, TestRemoveItemsNotifiesObservers) {
  Product mockProduct ("MockProduct", 123);
  InventoryTest underTest;
  underTest._addItems (mockProduct, 234);

  MockObserver<StockChangeMessage> mockObserver;
  underTest.registerObserver(mockObserver);
  EXPECT_CALL(mockObserver, notify);
  underTest._removeItems (mockProduct, 123);
  EXPECT_EQ (111, underTest.getQuantity (mockProduct));
}

TEST (TestInventoryImpl, TestRemoveItemsThrowsWhenItemIsOutOfStock) {
  InventoryTest underTest;
  Product mockProduct ("MockProduct", 123);
  underTest._addItems (mockProduct, 123);
  EXPECT_THAT (
    [&] () { underTest._removeItems (mockProduct, 234); },

    Throws<OutOfStockException> (Field (&OutOfStockException::product, Eq (mockProduct)))
  );
  EXPECT_EQ (123, underTest.getQuantity (mockProduct));
}

} // namespace catapult::testing

