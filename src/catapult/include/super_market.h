#ifndef __SUPER_MARKET_H__
#define __SUPER_MARKET_H__

#include "inventory_impl.h"
#include "catalogue_impl.h"
#include "command.h"
#include "inventory.h"


namespace catapult
{
class SuperMarket: public InventoryImpl, CatalogueImpl, public MultiReceiver<Catalogue, Inventory> {

};

class Basket: public InventoryImpl, public MultiReceiver<Inventory> {

};

} // namespace catapul


#endif // __SUPER_MARKET_H__
