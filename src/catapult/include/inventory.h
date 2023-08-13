#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include "catalogue.h"
#include "command.h"
#include "exception.h"
#include "observer.h"
#include "renderer.h"

namespace catapult
{

class OutOfStockException: public CatapultException {
  public:
    OutOfStockException(const Product& product): CatapultException("The product is out of stock"), product { product } {};

    Product product;
};

//! @brief a message which is sent to observers when a stock level changes.
class StockChangeMessage {
  public:
    StockChangeMessage( const Product& product, const uint32_t& newQuantity ): _product { product }, _newQuantity { newQuantity } {};
    Product getProduct() const { return _product; };
    uint32_t getNewQuantity() const { return _newQuantity; };

  private:
    Product _product;
    uint32_t _newQuantity;
};

//! @brief represents a ledger of product quantity;
class Inventory: public Renderable<Inventory>, public BaseObservable<StockChangeMessage> {
  public:
    ~Inventory() override = default;
    class AddItemsCommand;
    class RemoveItemsCommand;

    //! @brief returns the quantity of product currently held in the inventory
    //! @param product the product
    //! @returns the quantity of product held in the inventory
    virtual uint32_t getQuantity(Product product) const = 0;

    //! @brief returns the products which are held in the inventory
    //! @returns a std::vector of products which are held in the inventory
    virtual std::vector<Product> getProducts() const = 0;

    class MoveStockCommandFactory {
      public:
        static Transaction<Inventory> build(Inventory &source, Inventory &destination, const Product &product, uint32_t quantity);
    };

  protected:
    //! @brief add a quatity of product to the inventory
    //! @param product the product to add
    //! @param quantity the quantity to add
    virtual void _addItems(Product product, uint32_t quantity) = 0;

    //! @brief remove a quatity of product to the inventory
    //! @param product the product to add
    //! @param quantity the quantity to add
    virtual void _removeItems(Product product, uint32_t quantity) = 0;
};

//! Adds a quantity of product to the inventory on which it is executed;
class Inventory::AddItemsCommand: public UndoableBindableCommand<Inventory> {
  public:
    AddItemsCommand(const Product& product, uint32_t quantity): _product(product), _quantity(quantity) {};
    void undo(Inventory& inventory) const override;

  private:
    void _execute(Inventory& inventory) const override;
    Product _product;
    uint32_t _quantity;
};

//! Removes a quantity of product to the inventory on which it is executed;
class Inventory::RemoveItemsCommand: public UndoableBindableCommand<Inventory> {
  public:
    RemoveItemsCommand(const Product& product, uint32_t quantity): _product(product), _quantity(quantity) {};
    void undo(Inventory& inventory) const override;

  private:
    void _execute(Inventory& inventory) const override;
    Product _product;
    uint32_t _quantity;
};

} // namespace catapult

#endif // __INVENTORY_H__
