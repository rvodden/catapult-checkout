#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "inventory.h"

namespace catapult::testing {

using namespace ::testing;

class MockInventory: public Inventory {
  public:
    MOCK_METHOD (void, _addItems, (Product product, uint32_t Quantity), ());
    MOCK_METHOD (void, _removeItems, (Product product, uint32_t Quantity), ());
    MOCK_METHOD (uint32_t, getQuantity, (Product product), (const));
};

TEST (TestInventory, TestAddProductCommand) {
  MockInventory mockInventory {};
  Product mockProduct { "MockProduct", 123u };
  EXPECT_CALL (mockInventory, _addItems (mockProduct, 123));
  Inventory::AddItemsCommand addItems { mockProduct, 123 };
  addItems.execute (mockInventory);
};

TEST (TestInventory, TestUndoAddProductCommand) {
  MockInventory mockInventory {};
  Product mockProduct { "MockProduct", 123u };
  EXPECT_CALL (mockInventory, _addItems (mockProduct, 123));
  EXPECT_CALL (mockInventory, _removeItems (mockProduct, 123));
  Inventory::AddItemsCommand addItems { mockProduct, 123 };
  addItems.execute (mockInventory);
  addItems.undo (mockInventory);
};

TEST (TestInventory, TestRemoveProductCommand) {
  MockInventory mockInventory {};
  Product mockProduct { "MockProduct", 123u };
  EXPECT_CALL (mockInventory, _removeItems (mockProduct, 123));
  Inventory::RemoveItemsCommand removeItems { mockProduct, 123 };
  removeItems.execute (mockInventory);
};

TEST (TestInventory, TestUndoRemoveProductCommand) {
  MockInventory mockInventory {};
  Product mockProduct { "MockProduct", 123u };
  EXPECT_CALL (mockInventory, _removeItems (mockProduct, 123));
  EXPECT_CALL (mockInventory, _addItems (mockProduct, 123));
  Inventory::RemoveItemsCommand removeItems { mockProduct, 123 };
  removeItems.execute (mockInventory);
  removeItems.undo (mockInventory);
};

TEST (TestInventory, TestMoveStockCommand) {
  MockInventory source {};
  MockInventory destination {};

  Product mockProduct { "MockProduct", 123u };
  EXPECT_CALL(source, _removeItems(mockProduct, 123));
  EXPECT_CALL(destination, _addItems(mockProduct, 123));
  auto moveStock = Inventory::MoveStockCommandFactory::build(source, destination, mockProduct, 123);
  moveStock.execute();
};

}  // namespace catapult::testing
