#ifndef __INVENTORY_IMPORTER_H__
#define __INVENTORY_IMPORTER_H__

#include <filesystem>
#include <memory>
#include <tuple>
#include <unordered_map>

#include "catalogue.h"
#include "command.h"
#include "loader.h"

namespace catapult {

using InterpretedRecord = std::tuple<Product,ProductGroup>;

class InventoryInterpreter: public Interpreter<InterpretedRecord> {
  public:
    std::tuple<Product, ProductGroup> interpret (const Record &record) const override;

  private:
    enum class fields { kReference, kName, kUnitPrice, kProductGroup, kDealType, kQuantity };
    using enum fields;

    static const std::unordered_map<fields, uint8_t> kFieldColumnMap;
};

/*! @brief combines a Loader, Parser and Interpreter to produce a list of commands
 * from a data source */
class InventoryImporter {
  public:
    InventoryImporter(Loader *loader, Parser *parser, Interpreter<InterpretedRecord> *interpreter):
      _loader ( loader ),
      _parser ( parser ),
      _interpreter ( interpreter ) {};

    InventoryImporter (const InventoryImporter&) = delete;
    InventoryImporter& operator=(InventoryImporter&) = delete;
    virtual ~InventoryImporter() {};

    [[nodiscard]] CommandList<Catalogue> import () const;

  private:
    std::unique_ptr<Loader> _loader;
    std::unique_ptr<Parser> _parser;
    std::unique_ptr<Interpreter<InterpretedRecord>> _interpreter;
};

class CSVFileInventoryImporter: public InventoryImporter {
  public:
    CSVFileInventoryImporter (const std::filesystem::path &path) noexcept:
      InventoryImporter (new FileLoader (path), new CSVParser (), new InventoryInterpreter ()) {};
};

}  // namespace catapult

#endif  // __INVENTORY_IMPORTER_H__
