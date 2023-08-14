#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "catalogue.h"

namespace catapult::testing {
using namespace ::testing;


class MockCatalogue: public Catalogue {
  public:
    MOCK_METHOD (void, _addProduct, (const Product &product), (override));
    MOCK_METHOD (void, _addProductGroup, (const ProductGroup &productGroup), (override));
    MOCK_METHOD (Product, getProductByName, (const std::string &), (override));
};

TEST (TestSuperMarket, TestAddProductCommand) {
  MockCatalogue mockCatalogue;
  Product mockProduct { "MockProduct", 123u };
  EXPECT_CALL (mockCatalogue, _addProduct (mockProduct));
  Catalogue::AddProductCommand addProduct { mockProduct };
  addProduct.execute (mockCatalogue);
};

TEST (TestSuperMarket, AddTestAddProductGroupCommand) {
  MockCatalogue mockCatalogue;
  ProductGroup mockProductGroup { "MockProductGroup" };
  EXPECT_CALL (mockCatalogue, _addProductGroup (mockProductGroup));
  Catalogue::AddProductGroupCommand addProductGroup { mockProductGroup };
  addProductGroup.execute (mockCatalogue);
};

TEST (TestProduct, TestComparisonOperator) {
  Product mockProduct1 { "MockProduct", 123 };
  Product mockProduct2 { "MockProduct", 123 };
  Product mockProduct3 { "MockProduct", 321 };
  Product mockProduct4 { "MockProduct1", 123 };

  EXPECT_TRUE( mockProduct1 == mockProduct1 );
  EXPECT_TRUE( mockProduct1 == mockProduct2 );
  EXPECT_FALSE( mockProduct1 == mockProduct3 );
  EXPECT_FALSE( mockProduct1 == mockProduct4 );
};

TEST (TestProduct, TestLTOperator) {
  Product mockProduct1 { "MockProduct", 123 };
  Product mockProduct2 { "MockProduct", 12 };
  Product mockProduct3 { "MockProduct1", 1212 };

  EXPECT_FALSE( mockProduct1 < mockProduct1 );
  EXPECT_FALSE( mockProduct1 < mockProduct2 );
  EXPECT_TRUE( mockProduct2 < mockProduct1 );
  EXPECT_TRUE( mockProduct1 < mockProduct3 );
}

}  // namespace catapult::testing
