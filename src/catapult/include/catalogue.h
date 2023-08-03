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

    bool operator== (const Product &other) const noexcept {
      return other._name == _name && other._unitCost == _unitCost;
    };

    friend std::ostream &operator<< (std::ostream &outStream, const Product &product);

  private:
    std::string _name;
    uint32_t _unitCost;
};

std::ostream &operator<< (std::ostream &outStream, const Product &product);

//! @brief represents a group of products
class ProductGroup {};

class Catalogue: public Receiver<Catalogue> {
  public:
    virtual void addProduct (const Product &product) = 0;
};

class AddProductCommand: public Executable<Catalogue> {
  public:
    AddProductCommand (const std::string &name, const uint32_t &unitCost): _name (name), _unitCost (unitCost) {};
    void execute (Catalogue &catalogue) const override;

  private:
    std::string _name;
    uint32_t _unitCost;
};

}  // namespace catapult

#endif  // __CATALOGUE_H__
