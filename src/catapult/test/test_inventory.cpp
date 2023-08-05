#include "inventory.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace catapult::testing
{

class MockInventory: public Inventory {
  public:
    MOCK_METHOD(void, addItems, (Product product, uint32_t Quantity), ());
    MOCK_METHOD(uint32_t, getQuantity, (Product product), (const));
};

TEST (TestInventory, TestAddProductCommand) {
  MockInventory mockInventory {};
  Product mockProduct { "MockProduct", 123u };
  EXPECT_CALL (mockInventory, addItems (mockProduct, 123));
  Inventory::AddItemsCommand addItems { mockProduct, 123 };
  addItems.execute (mockInventory);
};

class InventoryTest: public InventoryImpl {
  FRIEND_TEST(TestInventory, TestAddProductAndGetQuantity);
};

TEST(TestInventory, TestAddProductAndGetQuantity) {
  InventoryTest underTest;
  Product mockProduct("MockProduct", 123);
  underTest.addItems(mockProduct, 234);
  EXPECT_EQ(234, underTest.getQuantity(mockProduct));
}

TEST(TestInventory, TestGetQuantityWhenOutOfStock) {
  InventoryImpl underTest;
  Product mockProduct("MockProduct", 123);
  EXPECT_EQ(0, underTest.getQuantity(mockProduct));
}

} // namespace catapult::testing

