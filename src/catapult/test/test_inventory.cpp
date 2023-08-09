#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "inventory.h"

namespace catapult::testing {

using namespace ::testing;

class MockInventory: public Inventory {
  public:
    MOCK_METHOD (void, addItems, (Product product, uint32_t Quantity), ());
    MOCK_METHOD (void, removeItems, (Product product, uint32_t Quantity), ());
    MOCK_METHOD (uint32_t, getQuantity, (Product product), (const));
};

TEST (TestInventory, TestAddProductCommand) {
  MockInventory mockInventory {};
  Product mockProduct { "MockProduct", 123u };
  EXPECT_CALL (mockInventory, addItems (mockProduct, 123));
  Inventory::AddItemsCommand addItems { mockProduct, 123 };
  addItems.execute (mockInventory);
};

TEST (TestInventory, TestRemoveProductCommand) {
  MockInventory mockInventory {};
  Product mockProduct { "MockProduct", 123u };
  EXPECT_CALL (mockInventory, removeItems (mockProduct, 123));
  Inventory::RemoveItemsCommand removeItems { mockProduct, 123 };
  removeItems.execute (mockInventory);
};

class InventoryTest: public InventoryImpl {
    FRIEND_TEST (TestInventory, TestAddItemsAndGetQuantity);
    FRIEND_TEST (TestInventory, TestRemoveItems);
    FRIEND_TEST (TestInventory, TestRemoveItemsThrowsWhenItemIsOutOfStock);
};

TEST (TestInventory, TestAddItemsAndGetQuantity) {
  InventoryTest underTest;
  Product mockProduct ("MockProduct", 123);
  underTest.addItems (mockProduct, 234);
  EXPECT_EQ (234, underTest.getQuantity (mockProduct));
}

TEST (TestInventory, TestGetQuantityWhenOutOfStock) {
  InventoryImpl underTest;
  Product mockProduct ("MockProduct", 123);
  EXPECT_EQ (0, underTest.getQuantity (mockProduct));
}

TEST (TestInventory, TestRemoveItems) {
  InventoryTest underTest;
  Product mockProduct ("MockProduct", 123);
  underTest.addItems (mockProduct, 234);
  underTest.removeItems (mockProduct, 123);
  EXPECT_EQ (111, underTest.getQuantity (mockProduct));
}

TEST (TestInventory, TestRemoveItemsThrowsWhenItemIsOutOfStock) {
  InventoryTest underTest;
  Product mockProduct ("MockProduct", 123);
  underTest.addItems (mockProduct, 123);
  EXPECT_THAT (
    [&] () { underTest.removeItems (mockProduct, 234); },

    Throws<OutOfStockException> (Field (&OutOfStockException::product, Eq (mockProduct)))
  );
  EXPECT_EQ (123, underTest.getQuantity (mockProduct));
}

}  // namespace catapult::testing
