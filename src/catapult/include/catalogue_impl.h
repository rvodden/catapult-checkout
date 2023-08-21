#ifndef __SUPERMARKET_H__
#define __SUPERMARKET_H__

#include <memory>
#include <unordered_map>
#include <vector>

#include "catalogue.h"
#include "command.h"

namespace catapult {

//! @brief an implementation of the Catalogue interface
class CatalogueImpl: virtual public Catalogue {
  public:
    std::vector<Product> getProducts () const;
    Product getProductByName(const std::string& name) const override;
    std::vector<std::reference_wrapper<const Product>> getProductsInProductGroup(const ProductGroup& productGroup) const override;

  protected:
    void _addProduct (const Product &product) override;
    void _addProductGroup (const ProductGroup &productGroup) override;
    void _addProductToGroup (const Product& product, const ProductGroup &productGroup) override;

  private:
    std::vector<Product> _products;
    std::vector<ProductGroup> _productGroups;
    std::unordered_map<
      std::reference_wrapper<const ProductGroup>,
      std::vector<std::reference_wrapper<const Product>>,
      std::hash<std::reference_wrapper<ProductGroup>>,
      std::equal_to<catapult::ProductGroup>
      > _productGroupProductMap;
};

}

#endif
