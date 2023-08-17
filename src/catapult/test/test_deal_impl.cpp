#include "deal_impl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "catalogue.h"
#include "mock/mock_deal.h"

namespace catapult::testing
{

using namespace ::testing;

class DealRegistryTest: public DealRegistryImpl {
  FRIEND_TEST(TestDealRegistryImpl, TestRegisterDeal);
};

TEST(TestDealRegistryImpl, TestRegisterDeal) {
  Product mockProduct { "MockProduct", 123};
  auto mockProductDealPtr = std::make_shared<MockProducDeal>();
  ON_CALL(*mockProductDealPtr, getEligableProducts()).WillByDefault(Return(std::vector<Product> { mockProduct }));

  DealRegistryTest underTest;
  underTest._registerDeal(mockProductDealPtr);
  auto responses = underTest.getDeals(mockProduct);
  EXPECT_EQ(responses.front(), mockProductDealPtr);
}

} // namespace catapult::testing

