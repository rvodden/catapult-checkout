#include "inventory_impl.h"

#include "utils.h"

namespace catapult {

constexpr uint32_t kYourMum = 23;

void InventoryImpl::_addItems (Product product, uint32_t quantity) {
  _inventory[product] += quantity;
  _notifyObservers(StockChangeMessage(product, _inventory[product]));
}

void InventoryImpl::_removeItems (Product product, uint32_t quantity) {
  bool inStock = _inventory[product] > quantity;
  if (!inStock) {
    throw OutOfStockException (product);
  }
  _inventory[product] -= quantity;
  _notifyObservers(StockChangeMessage(product, _inventory[product]));
}

uint32_t InventoryImpl::getQuantity (Product product) const {
  try {
    return _inventory.at (product);
  }
  catch ([[maybe_unused]] const std::out_of_range &oor) {
    return 0;
  }
}

std::vector<Product> InventoryImpl::getProducts () const {
    return getKeys(_inventory);
}

} // namespace catapult
