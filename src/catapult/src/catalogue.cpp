#include "catalogue.h"

namespace std {

std::size_t std::hash<catapult::Product>::operator() (const catapult::Product &product) const noexcept {
  return std::hash<std::string> {}(product._name);
}

std::size_t std::hash<catapult::ProductGroup>::operator() (const catapult::ProductGroup &productGroup) const noexcept {
  return std::hash<std::string> {}(productGroup._name);
}

}

namespace catapult {

void Catalogue::AddProductCommand::_execute (Catalogue &catalogue) const {
  catalogue._addProduct (_product);
}

std::ostream &operator<< (std::ostream &outStream, const Product &product) {
  outStream << "Product(\"" << product._name << "\", " << product._unitCost << ")";
  return outStream;
}

std::ostream &operator<< (std::ostream &outStream, const ProductGroup &productGroup) {
  outStream << "ProductGroup(\"" << productGroup._name << "\")";
  return outStream;
};

void Catalogue::AddProductGroupCommand::_execute (Catalogue &catalogue) const {
  catalogue._addProductGroup (_productGroup);
}

}  // namespace catapult
