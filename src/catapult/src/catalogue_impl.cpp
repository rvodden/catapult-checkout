#include "catalogue_impl.h"

namespace catapult {

std::vector<Product> CatalogueImpl::getProducts () const {
  return _products;
}

Product CatalogueImpl::getProductByName (const std::string &name) const {
  auto product = std::find_if (_products.cbegin (), _products.cend (), [&name] (const Product &product) {
    return (product.getName () == name);
  });
  if (product == _products.cend ()) {
    throw ProductNotFoundException ("Product cannot be found");
  }
  return *product;
}

std::vector<std::reference_wrapper<const Product>> CatalogueImpl::getProductsInProductGroup (const ProductGroup &productGroup) const {
  try {
    return _productGroupProductMap.at(std::ref(productGroup));
  } catch( const std::out_of_range& oor ) {
    throw ProductGroupNotFoundException("Product group cannot be found");
  }
}

void CatalogueImpl::_addProduct (const Product &product) {
  _products.push_back (product);
}

void CatalogueImpl::_addProductGroup (const ProductGroup &productGroup) {
  _productGroups.push_back (productGroup);
}

void CatalogueImpl::_addProductToGroup (const Product& product, const ProductGroup &productGroup) {
  auto& productVector = _productGroupProductMap[std::ref(productGroup)];
  productVector.push_back(std::ref(product));
}

}
