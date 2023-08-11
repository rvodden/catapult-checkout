#include "inventory_impl.h"

namespace catapult {

void InventoryImpl::_addItems (Product product, uint32_t quantity) {
  _inventory[product] += quantity;
}

void InventoryImpl::_removeItems (Product product, uint32_t quantity) {
  bool inStock = _inventory[product] > quantity;
  if (!inStock)
    throw OutOfStockException (product);
  _inventory[product] -= quantity;
}

uint32_t InventoryImpl::getQuantity (Product product) const {
  try {
    return _inventory.at (product);
  }
  catch ([[maybe_unused]] const std::out_of_range &oor) {
    return 0;
  }
}

} // namespace catapult
