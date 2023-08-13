#ifndef __MOCK_INVENTORY_H__
#define __MOCK_INVENTORY_H__

#include <gmock/gmock.h>

#include "inventory.h"

namespace catapult::testing
{

using namespace ::testing;

class MockInventory: public Inventory {
  public:
    MOCK_METHOD (void, _addItems, (Product product, uint32_t Quantity), (override));
    MOCK_METHOD (void, _removeItems, (Product product, uint32_t Quantity), (override));
    MOCK_METHOD (uint32_t, getQuantity, (Product product), (const override));
    MOCK_METHOD (std::vector<Product>, getProducts, (), (const override));
};

} // namespace catapult::testing

#endif // __MOCK_INVENTORY_H__
