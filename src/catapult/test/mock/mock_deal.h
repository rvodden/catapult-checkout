#ifndef __MOCK_MOCK_DEAL_H__
#define __MOCK_MOCK_DEAH_H__

#include <deal.h>

namespace catapult::testing {

class MockProducDeal: public ProductDeal {
  public:
    MockProducDeal() = default;
    MockProducDeal([[ maybe_unused ]] const MockProducDeal& foo) {};
    MockProducDeal([[ maybe_unused ]] MockProducDeal&& foo) {};
    MOCK_METHOD(std::vector<std::shared_ptr<Discount>>, getDiscounts, (), (const override) );
    MOCK_METHOD(std::vector<Product>, getEligableProducts, (), (const override) );
};

}

#endif
