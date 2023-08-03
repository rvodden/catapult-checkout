#include "catalogue.h"

namespace catapult {

void Catalogue::AddProductCommand::execute (Catalogue &catalogue) const {
  catalogue.addProduct (_product);
}

std::ostream &operator<< (std::ostream &outStream, const Product &product) {
  outStream << "Product(\"" << product._name << "\", " << product._unitCost << ")";
  return outStream;
}

std::ostream &operator<< (std::ostream &outStream, const ProductGroup &productGroup) {
  outStream << "ProductGroup(\"" << productGroup._name << "\")";
  return outStream;
};

void Catalogue::AddProductGroupCommand::execute (Catalogue &catalogue) const {
  catalogue.addProductGroup (_productGroup);
}

}  // namespace catapult
