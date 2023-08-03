#include "super_market.h"

namespace catapult {

std::vector<Product> SuperMarket::getProducts () const {
  return _products;
}

void SuperMarket::addProduct (const Product &product) {
  _products.push_back(product);
}

}
