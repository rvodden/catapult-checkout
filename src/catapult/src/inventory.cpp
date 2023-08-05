#include "inventory.h"

#include <stdexcept>


namespace catapult
{

void Inventory::AddItemsCommand::execute (Inventory& inventory) const {
  inventory.addItems(_product, _quantity);
}

void InventoryImpl::addItems (Product product, uint32_t quantity) {
  _inventory[product] += quantity;
};

uint32_t InventoryImpl::getQuantity(Product product) const {
  try {
    return _inventory.at(product);
  } catch ([[maybe_unused]] const std::out_of_range& oor) {
    return 0;
  }
}

}  // namespace catapult
