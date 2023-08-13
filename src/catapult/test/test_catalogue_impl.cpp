#include "catalogue_impl.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace catapult::testing
{

using namespace ::testing;

class CatalogueImplTest : public CatalogueImpl {
  FRIEND_TEST(TestCatalogueImpl, TestAddProduct);
  FRIEND_TEST(TestCatalogueImpl, TestGetProductByName);
  FRIEND_TEST(TestCatalogueImpl, TestGetProductByNameThrowsWhenProductNotFound);
};

TEST(TestCatalogueImpl, TestAddProduct) {
  Product mockProduct {"ProductName", 123};
  CatalogueImplTest underTest;
  underTest._addProduct(mockProduct);
}

TEST(TestCatalogueImpl, TestGetProductByName) {
  Product mockProduct {"ProductName", 123};
  CatalogueImplTest underTest;
  underTest._addProduct(mockProduct);
  auto foundProduct = underTest.getProductByName("ProductName");
  EXPECT_THAT(foundProduct, Eq(mockProduct));
}

TEST(TestCatalogueImpl, TestGetProductByNameThrowsWhenProductNotFound) {
  Product mockProduct {"ProductName", 123};
  CatalogueImplTest underTest;
  underTest._addProduct(mockProduct);
  EXPECT_THAT([&](){ underTest.getProductByName("UnfoundProductName"); }, Throws<ProductNotFoundException>() );
}


} // namespace catapult::testing
