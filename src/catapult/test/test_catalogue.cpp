#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "catalogue.h"

namespace catapult::testing {
using namespace ::testing;


class MockCatalogue: public Catalogue {
  public:
    MOCK_METHOD (void, addProduct, (const Product &product), ());
};

TEST (TestSuperMarket, AddProductTest) {
  MockCatalogue mockCatalogue;
  EXPECT_CALL (mockCatalogue, addProduct (Product ("MockProduct", 123u)));
  AddProductCommand addProduct { "MockProduct", 123 };
  addProduct.execute (mockCatalogue);
};

}  // namespace catapult::testing
