#include "catalogue_impl.h"

#include <gtest/gtest.h>

namespace catapult::testing
{

class CatalogueImplTest : public CatalogueImpl {
  FRIEND_TEST(TestCatalogueImpl, TestAddProduct);
};

TEST(TestCatalogueImpl, TestAddProduct) {
  Product mockProduct {"ProductName", 123};
  CatalogueImplTest underTest;
  underTest._addProduct(mockProduct);
}

} // namespace catapult::testing
