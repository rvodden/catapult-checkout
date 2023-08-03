#include "inventory_importer.h"

#include <cmath>
#include <memory>
#include <unordered_set>

#include "catalogue.h"

namespace catapult {

std::tuple<Product,ProductGroup> InventoryInterpreter::interpret (const Record &record) const {
  using enum fields;
  Product product {
    record[static_cast<uint8_t> (kName)],
    std::lround( std::stof (record[static_cast<uint8_t> (kUnitPrice)]) * 100. )
  };
  ProductGroup productGroup {
    record[static_cast<u_int8_t>(kProductGroup)]
  };

  return std::make_tuple(product, productGroup);
}

const std::unordered_map<InventoryInterpreter::fields, uint8_t> InventoryInterpreter::kFieldColumnMap {
  { kReference, 0 },
  { kName, 1 },
  { kUnitPrice, 2},
  { kProductGroup, 3},
  { kDealType, 4},
  { kQuantity, 5}
};

[[nodiscard]] CommandList<Catalogue> InventoryImporter::import () const {
  auto records = _parser->parse (_loader->load ());
  // std::vector<Product> products;
  CommandList<Catalogue> commands {};
  // std::unordered_set<ProductGroup> productGroups;
  for (const auto& record: records) {
    auto [product, productGroup] = _interpreter->interpret (record);
    commands.push_back(std::make_shared<Catalogue::AddProductCommand>(product));
    commands.push_back(std::make_shared<Catalogue::AddProductGroupCommand>(productGroup));
  }
  return commands;
};


}  // namespace catapult
