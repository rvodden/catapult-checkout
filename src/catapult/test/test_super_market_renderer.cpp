#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>

#include "catalogue.h"
#include "inventory.h"
#include "mock/mock_inventory.h"
#include "super_market_renderer.h"

namespace catapult::testing {

using namespace ::testing;

class Line {
    std::string data;

  public:
    friend std::istream &operator>> (std::istream &is, Line &l) {
      std::getline (is, l.data);
      return is;
    }
    operator std::string () const { return data; }
};

TEST (TestRenderInventory, TestRenderInventory) {
  MockInventory mockInventory;
  auto banana = Product ("Banana", 123);
  auto cheddar = Product ("Cheddar", 234);
  auto bread = Product ("Bread", 345);
  std::vector<Product> productList = { banana, cheddar, bread };

  ON_CALL (mockInventory, getProducts ()).WillByDefault (Return (productList));
  ON_CALL (mockInventory, getQuantity (banana)).WillByDefault (Return (5));
  ON_CALL (mockInventory, getQuantity (cheddar)).WillByDefault (Return (10));
  ON_CALL (mockInventory, getQuantity (bread)).WillByDefault (Return (12));

  std::stringstream stringStream;
  TextRenderer underTest { stringStream };
  underTest.render (mockInventory);
  std::for_each(
    std::istream_iterator<Line>(stringStream),
    std::istream_iterator<Line>(),
    [](const std::string& line){ EXPECT_THAT(line, MatchesRegex(R"_([[:space:]]*[[:alnum:]]+:[[:space:]]+[0-9]*[[:space:]]+@[[:space:]]+£[0-9]*\.[0-9]{2})_")); }
  );
}

}  // namespace catapult::testing
