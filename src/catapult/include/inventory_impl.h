#ifndef __INVENTORY_IMPL_H__
#define __INVENTORY_IMPL_H__

#include "inventory.h"

namespace catapult
{

//! @brief implementation of an inventory
class InventoryImpl: virtual public Inventory {
  public:
    uint32_t getQuantity(Product product) const override;
    std::vector<Product> getProducts() const;

  protected:
    void _addItems(Product product, uint32_t quantity) override;
    void _removeItems(Product product, uint32_t quantity) override;

  private:
    std::unordered_map<Product, uint32_t> _inventory;
};

} // namespace catapult


#endif // __INVENTORY_IMPL_H__
