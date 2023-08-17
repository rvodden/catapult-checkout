#ifndef __DEAL_IMPL_H__
#define __DEAL_IMPL_H__

#include "deal.h"

#include <unordered_map>

namespace catapult
{

class DealRegistryImpl: public DealRegistry {
  public:
    std::vector<std::shared_ptr<ProductDeal>> getDeals ( const Product& product ) const override;

  protected:
    void _registerDeal (std::shared_ptr<ProductDeal> deal) override;

  private:
    std::unordered_map<Product, std::vector<std::shared_ptr<ProductDeal>>> _productDealsMap;
};

} // namespace catapult


#endif
