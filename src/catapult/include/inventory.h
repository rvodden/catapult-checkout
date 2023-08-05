#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include "catalogue.h"
#include "command.h"
#include "exception.h"

namespace catapult
{

class OutOfStockException: public CatapultException {
  public:
    OutOfStockException(Product product): CatapultException("The product is out of stock"), _product { product } {};

  private:
    Product _product;
};

//! @brief represents a ledger of product quantity;
class Inventory {
  public:
    class AddItemsCommand;

  protected:
    //! @brief add a quatity of product to the inventory
    //! @param product the product to add
    //! @param quantity the quantity to add
    virtual void addItems(Product product, uint32_t quantity) = 0;

    //! @brief returns the quantity of product currently held in the inventory
    //! @param product the product
    //! @returns the quantity of product held in the inventory
    virtual uint32_t getQuantity(Product product) const = 0;
};

//! Adds a quantity of product to the inventory on which it is executed;
class Inventory::AddItemsCommand: public Executable<Inventory> {
  public:
    AddItemsCommand(Product product, uint32_t quantity): _product(product), _quantity(quantity) {};
    void execute(Inventory& inventory) const override;

  private:
    Product _product;
    uint32_t _quantity;
};

//! @brief implementation of an inventory
class InventoryImpl: virtual public Inventory {
  public:
    uint32_t getQuantity(Product product) const override;

  protected:
    void addItems(Product product, uint32_t quantity) override;

  private:
    std::unordered_map<Product, uint32_t> _inventory;
};


} // namespace catapult


#endif // __INVENTORY_H__