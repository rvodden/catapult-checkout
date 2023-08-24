#include "super_market_renderer.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

namespace catapult {

const static auto kLongestName = [] (const Product& p1, const Product& p2) { return p1.getName ().size () < p2.getName ().size (); };
const static auto kHighestCost = [] (const Product& p1, const Product& p2) { return p1.getUnitCost () < p2.getUnitCost (); };

void TextRenderer::render (const Inventory &inventory) const {
  auto products = inventory.getProducts ();
  auto longestNamedProduct = std::ranges::max_element (products, kLongestName);
  auto nameColumnWidth = longestNamedProduct->getName ().size () + 2;

  _outputStream.get().imbue (std::locale ("en_GB.utf8"));
  auto print = [=,this, &inventory, &nameColumnWidth] (const auto &product) {
    this->_outputStream.get() << std::right << std::setw (static_cast<int32_t>(nameColumnWidth)) << product.getName () << ": " << std::setw (3)
              << inventory.getQuantity (product) << " @" << std::right << std::setw (8) << std::showbase
              << std::put_money (static_cast<float> (product.getUnitCost ())) << "\n";
  };
  std::ranges::for_each (products, print);
};

}  // namespace catapult
