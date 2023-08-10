#include "catalogue_impl.h"

namespace catapult {

std::vector<Product> CatalogueImpl::getProducts () const {
  return _products;
}

void CatalogueImpl::_addProduct (const Product &product) {
  _products.push_back(product);
}

void CatalogueImpl::_addProductGroup (const ProductGroup &productGroup) {
  _productGroups.push_back(productGroup);
}
}
