#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "inventory.h"
#include "mock/mock_inventory.h"

namespace catapult::testing {

using namespace ::testing;

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
