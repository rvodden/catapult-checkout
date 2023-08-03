#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "catalogue.h"

namespace catapult::testing {
using namespace ::testing;


class MockCatalogue: public Catalogue {
  public:
    MOCK_METHOD (void, addProduct, (const Product &product), ());
    MOCK_METHOD (void, addProductGroup, (const ProductGroup &productGroup), ());
};

TEST (TestSuperMarket, AddProductTest) {
  MockCatalogue mockCatalogue;
  Product mockProduct { "MockProduct", 123u };
  EXPECT_CALL (mockCatalogue, addProduct (mockProduct));
  Catalogue::AddProductCommand addProduct { mockProduct };
  addProduct.execute (mockCatalogue);
};

}  // namespace catapult::testing
