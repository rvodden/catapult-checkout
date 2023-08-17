#include "deal_impl.h"

namespace catapult
{

std::vector<std::shared_ptr<ProductDeal>> DealRegistryImpl::getDeals(const Product& product) const {
  return _productDealsMap.at(product);
}

void DealRegistryImpl::_registerDeal (std::shared_ptr<ProductDeal> deal) {
  auto products = deal->getEligableProducts();

  for(const auto& product: products) {
    if(! _productDealsMap.contains(product)) {
      _productDealsMap[product] = std::vector { deal };
      continue;
    }

    _productDealsMap[product].push_back( deal );
  }
}

} // namespace catapult

