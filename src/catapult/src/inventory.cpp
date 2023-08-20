#include "inventory.h"

#include <stdexcept>

namespace catapult {

void Inventory::AddItemsCommand::_execute (Inventory &inventory) {
  inventory._addItems (_product, _quantity);
}

void Inventory::AddItemsCommand::undo (Inventory &inventory) {
  inventory._removeItems (_product, _quantity);
}

void Inventory::RemoveItemsCommand::_execute (Inventory &inventory) {
  inventory._removeItems (_product, _quantity);
}

void Inventory::RemoveItemsCommand::undo (Inventory &inventory) {
  inventory._addItems (_product, _quantity);
}

Transaction<Inventory> Inventory::MoveStockCommandFactory::build (
  Inventory &source,
  Inventory &destination,
  const Product &product,
  uint32_t quantity
) {
  Transaction<Inventory> transaction;
  auto removeItems = std::make_shared<Inventory::RemoveItemsCommand> (product, quantity);
  removeItems->bind (source);
  auto addItems = std::make_shared<Inventory::AddItemsCommand> (product, quantity);
  addItems->bind (destination);
  transaction.then<Inventory> (removeItems).then<Inventory> (addItems);
  return transaction;
}

}  // namespace catapult
