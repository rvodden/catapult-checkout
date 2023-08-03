#include "inventory_loader.h"

#include <cmath>
#include <memory>

#include "catalogue.h"

namespace catapult {

CommandList<Catalogue> InventoryInterpreter::interpret (const Record &record) const {
  using enum fields;
  CommandList<Catalogue> commandList;
  auto command = std::make_shared<AddProductCommand> (
    record[static_cast<uint8_t> (kName)],
    trunc( std::stof (record[static_cast<uint8_t> (kUnitPrice)]) * 100 )
  );
  commandList.push_back (command);
  return commandList;
}


}  // namespace catapult
