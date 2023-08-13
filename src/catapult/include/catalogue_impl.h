#ifndef __SUPERMARKET_H__
#define __SUPERMARKET_H__

#include <memory>
#include <vector>

#include "catalogue.h"
#include "command.h"

namespace catapult {

//! @brief an implementation of the Catalogue interface
class CatalogueImpl: virtual public Catalogue {
  public:
    std::vector<Product> getProducts () const;
    Product getProductByName(const std::string& name) override;

  protected:
    void _addProduct (const Product &product) override;
    void _addProductGroup (const ProductGroup &productGroup) override;

  private:
    std::vector<Product> _products;
    std::vector<ProductGroup> _productGroups;
};

}

#endif
