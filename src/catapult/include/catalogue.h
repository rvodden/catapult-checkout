#ifndef __CATALOGUE_H__
#define __CATALOGUE_H__

#include <cmath>
#include <cstdint>
#include <functional>
#include <ostream>
#include <string>

#include "command.h"

namespace catapult {

class Product;
class ProductGroup;

}

template<>
class std::hash<catapult::Product> {
  public:
    std::size_t operator()(const catapult::Product& product) const noexcept;
};

template<>
class std::hash<catapult::ProductGroup> {
  public:
    std::size_t operator()(const catapult::ProductGroup& productGroup) const noexcept;
};

namespace catapult {

//! @brief represents a item which can be stocked and sold
class Product {
  public:
    Product (const std::string &name, const uint32_t &unitCost) noexcept: _name (name), _unitCost (unitCost) {};

    bool operator==(const Product&) const = default;

    friend std::ostream &operator<< (std::ostream &outStream, const Product &product);
    friend std::size_t std::hash<Product>::operator()(const Product& product) const noexcept;

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
    friend std::size_t std::hash<ProductGroup>::operator()(const ProductGroup& group) const noexcept;
  private:
    std::string _name;
};

std::ostream &operator<< (std::ostream &outStream, const ProductGroup &product);

//! @brief reprensents the products and groups of products which can be bought and sold
class Catalogue {
  public:
    virtual ~Catalogue() = default;
    class AddProductCommand;
    class AddProductGroupCommand;

  private:
    virtual void addProduct (const Product &product) = 0;
    virtual void addProductGroup (const ProductGroup &productGroup) = 0;
};

class Catalogue::AddProductCommand: public Command<Catalogue> {
  public:
    AddProductCommand (const Product& product): _product(product) {};
    void execute (Catalogue &catalogue) const override;

  private:
    Product _product;
};

class Catalogue::AddProductGroupCommand: public Command<Catalogue> {
  public:
    AddProductGroupCommand (const ProductGroup& productGroup): _productGroup(productGroup) {};
    void execute (Catalogue &catalogue) const override;

  private:
    ProductGroup _productGroup;
};

}  // namespace catapult



#endif  // __CATALOGUE_H__
