#ifndef __CATALOGUE_H__
#define __CATALOGUE_H__

#include <cmath>
#include <cstdint>
#include <ostream>
#include <string>

#include "command.h"

namespace catapult {

//! @brief represents a item which can be stocked and sold
class Product {
  public:
    Product (const std::string &name, const uint32_t &unitCost) noexcept: _name (name), _unitCost (unitCost) {};

    bool operator==(const Product&) const = default;

    friend std::ostream &operator<< (std::ostream &outStream, const Product &product);

  private:
    std::string _name;
    uint32_t _unitCost;
};

std::ostream &operator<< (std::ostream &outStream, const Product &product);

//! @brief represents a group of products
class ProductGroup {
  public:
    explicit ProductGroup(const std::string& name): _name(name) {};

    bool operator==(const ProductGroup&) const = default;
    friend std::ostream &operator<< (std::ostream &outStream, const ProductGroup &product);
  private:
    std::string _name;
};

std::ostream &operator<< (std::ostream &outStream, const ProductGroup &product);

class Catalogue: public Receiver<Catalogue> {
  public:
    class AddProductCommand;
    class AddProductGroupCommand;

  private:
    virtual void addProduct (const Product &product) = 0;
    virtual void addProductGroup (const ProductGroup &productGroup) = 0;
};

class Catalogue::AddProductCommand: public Executable<Catalogue> {
  public:
    AddProductCommand (const Product& product): _product(product) {};
    void execute (Catalogue &catalogue) const override;

  private:
    Product _product;
};

class Catalogue::AddProductGroupCommand: public Executable<Catalogue> {
  public:
    AddProductGroupCommand (const ProductGroup& productGroup): _productGroup(productGroup) {};
    void execute (Catalogue &catalogue) const override;

  private:
    ProductGroup _productGroup;
};

}  // namespace catapult

#endif  // __CATALOGUE_H__
