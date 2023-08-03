#ifndef __SUPERMARKET_H__
#define __SUPERMARKET_H__

#include <memory>
#include <vector>

#include "catalogue.h"
#include "command.h"

namespace catapult {

class SuperMarket: public Catalogue {
  public:
    std::vector<Product> getProducts () const;

  private:
    void addProduct (const Product &product) override;
    void addProductGroup (const ProductGroup &productGroup) override;
    std::vector<Product> _products;
    std::vector<ProductGroup> _productGroups;
};

}

#endif
