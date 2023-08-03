#ifndef __INVENTORY_LOADER_H__
#define __INVENTORY_LOADER_H__

#include <filesystem>

#include "catalogue.h"
#include "loader.h"

namespace catapult {

class InventoryInterpreter: public Interpreter<Catalogue> {
  public:
    CommandList<Catalogue> interpret (const Record &record) const override;

  private:
    enum class fields { kReference = 0, kName = 1, kUnitPrice = 2, kProductGroup = 3, kDealType = 4, kQuantity = 5 };
};

class CSVFileInventoryImporter: public Importer<Catalogue> {
  public:
    CSVFileInventoryImporter (const std::filesystem::path &path):
      Importer (new FileLoader (path), new CSVParser (), new InventoryInterpreter ()) {};
};

}  // namespace catapult

#endif  // __INVENTORY_LOADER_H__
