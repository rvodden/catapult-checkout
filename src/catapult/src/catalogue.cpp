#include "catalogue.h"

namespace catapult {

void AddProductCommand::execute (Catalogue &catalogue) const {
  catalogue.addProduct (Product (_name, _unitCost));
}

std::ostream &operator<< (std::ostream &outStream, const Product &product) {
  outStream << "Product(\"" << product._name << "\", " << product._unitCost << ")";
  return outStream;
}

}  // namespace catapult
