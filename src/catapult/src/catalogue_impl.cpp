#include "catalogue_impl.h"

namespace catapult {

std::vector<Product> CatalogueImpl::getProducts () const {
  return _products;
}

void CatalogueImpl::addProduct (const Product &product) {
  _products.push_back(product);
}

void CatalogueImpl::addProductGroup (const ProductGroup &productGroup) {
  _productGroups.push_back(productGroup);
}
}
