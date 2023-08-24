#include "stock_list_importer.h"

#include <cmath>
#include <forward_list>
#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_set>

#include "catalogue.h"
#include "inventory.h"

namespace catapult {

StockListRecord StockListInterpreter::interpret (const Record &record) const {
  using enum fields;
  Product product { record[kFieldColumnMap.at (kName)],
                    static_cast<uint32_t> (std::lroundf (std::stof (record[kFieldColumnMap.at (kUnitPrice)]) * 100.)) };
  ProductGroup productGroup { record[static_cast<u_int8_t> (kProductGroup)] };
  uint32_t quantity = std::stoi(record[kFieldColumnMap.at(kQuantity)]);

  return std::make_tuple (product, productGroup, quantity);
}

const std::unordered_map<StockListInterpreter::fields, uint8_t> StockListInterpreter::kFieldColumnMap {
  { kReference, 0 }, { kName, 1 }, { kUnitPrice, 2 }, { kProductGroup, 3 }, { kDealType, 4 }, { kQuantity, 5 }
};

[[nodiscard]] CommandList<Catalogue, Inventory> StockListImporter::import () const {

  auto records = _parser->parse (_loader->load ());
  CommandList<Catalogue, Inventory> commands {};
  std::unordered_map<Product, uint32_t> products;
  std::unordered_set<ProductGroup> productGroups;
  CommandList<Catalogue, Inventory> groupPopulationCommands {};

  for (const auto &record: records) {
    auto [product, productGroup, quantity] = _interpreter->interpret (record);
    products[product] += quantity;
    productGroups.insert (productGroup);
    groupPopulationCommands.push_back(std::make_shared<Catalogue::AddProductToGroupCommand> (product, productGroup));
  }

  for (const auto &productGroup: productGroups) {
    commands.push_back(std::make_shared<Catalogue::AddProductGroupCommand> (productGroup));
  }

  for (const auto &[product, quantity]: products) {
    commands.push_back(std::make_shared<Catalogue::AddProductCommand> (product));
    commands.push_back(std::make_shared<Inventory::AddItemsCommand> (product, quantity));
  }

  commands.insert(
    commands.end(),
    std::make_move_iterator(groupPopulationCommands.begin()),
    std::make_move_iterator(groupPopulationCommands.end())
  );

  return commands;
};


}  // namespace catapult
