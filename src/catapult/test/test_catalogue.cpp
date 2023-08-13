#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "catalogue.h"

namespace catapult::testing {
using namespace ::testing;


class MockCatalogue: public Catalogue {
  public:
    MOCK_METHOD (void, _addProduct, (const Product &product), ());
    MOCK_METHOD (void, _addProductGroup, (const ProductGroup &productGroup), ());
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

}  // namespace catapult::testing
