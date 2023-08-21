#include "catalogue.h"

namespace std {

std::size_t std::hash<catapult::Product>::operator() (const catapult::Product &product) const noexcept {
  return std::hash<std::string> {}(product._name);
}

std::size_t std::hash<catapult::ProductGroup>::operator() (const catapult::ProductGroup &productGroup) const noexcept {
  return std::hash<std::string> {}(productGroup._name);
}

std::size_t std::hash<std::reference_wrapper<catapult::ProductGroup>>::operator() (
  const std::reference_wrapper<const catapult::ProductGroup> &productGroup
) const noexcept {
  return std::hash<std::string> {}(productGroup.get ()._name);
}

}

namespace catapult {

void Catalogue::AddProductCommand::_execute (Catalogue &catalogue) {
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

void Catalogue::AddProductGroupCommand::_execute (Catalogue &catalogue) {
  catalogue._addProductGroup (_productGroup);
}

void Catalogue::AddProductToGroupCommand::_execute (Catalogue &catalogue) {
  catalogue._addProductToGroup (_product, _productGroup);
}

}  // namespace catapult
