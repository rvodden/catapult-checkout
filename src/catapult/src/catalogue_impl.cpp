#include "catalogue_impl.h"

namespace catapult {

std::vector<Product> CatalogueImpl::getProducts () const {
  return _products;
}

Product CatalogueImpl::getProductByName (const std::string &name) {
  auto product = std::find_if (_products.cbegin (), _products.cend (), [&name] (const Product &product) {
    return (product.getName () == name);
  });
  if (product == _products.cend ())
    throw ProductNotFoundException ("Product cannot be found");
  return *product;
}

void CatalogueImpl::_addProduct (const Product &product) {
  _products.push_back (product);
}

void CatalogueImpl::_addProductGroup (const ProductGroup &productGroup) {
  _productGroups.push_back (productGroup);
}

}
