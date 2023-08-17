#include "deal.h"

#include <algorithm>
#include <numeric>

#include "utils.h"

namespace catapult {

uint32_t MultiplePurchaseDiscount::getValue () const {
  return _product.getUnitCost () * _quantity;
}

MultiplePurchaseDeal::MultiplePurchaseDeal (const std::vector<Product> &products) {
  std::ranges::for_each (products, [&] (const auto &product) { _eligableProducts[product] = 0; });
};

uint32_t MultiplePurchaseDeal::size () const {
  return std::accumulate (_eligableProducts.begin (), _eligableProducts.end (), 0, [] (auto left, auto right) {
    return left + right.second;
  });
}

void MultiplePurchaseDeal::notify (StockChangeMessage stockChangeMessage) {
  _eligableProducts.at (stockChangeMessage.getProduct ()) = stockChangeMessage.getNewQuantity ();
}

std::vector<Product> MultiplePurchaseDeal::getEligableProducts () const {
  return getKeys(_eligableProducts);
}

std::vector<std::shared_ptr<Discount>> MultiplePurchaseDeal::getDiscounts () const {
  auto numberOfProductsRemainingToDiscount = size () / _multiplicity;
  std::vector<std::shared_ptr<Discount>> discounts;

  for (const auto &[product, quantity]: _eligableProducts) {
    if (quantity > numberOfProductsRemainingToDiscount) {
      discounts.push_back (std::make_shared<MultiplePurchaseDiscount> (product, numberOfProductsRemainingToDiscount));
      break;
    }
    discounts.push_back (std::make_shared<MultiplePurchaseDiscount> (product, quantity));
    numberOfProductsRemainingToDiscount -= quantity;
  }

  return discounts;
}

}  // namespace catapult
