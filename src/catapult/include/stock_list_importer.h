#ifndef __INVENTORY_IMPORTER_H__
#define __INVENTORY_IMPORTER_H__

#include <filesystem>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <variant>

#include "deal.h"
#include "catalogue.h"
#include "command.h"
#include "inventory.h"
#include "loader.h"

namespace catapult {

using StockListRecord = std::tuple<Product,ProductGroup,uint32_t>;

class StockListInterpreter: public Interpreter<StockListRecord> {
  public:
    StockListRecord interpret (const Record &record) const override;

  private:
    enum class fields { kReference, kName, kUnitPrice, kProductGroup, kDealType, kQuantity };
    using enum fields;

    static const std::unordered_map<fields, uint8_t> kFieldColumnMap;
};

/*! @brief combines a Loader, Parser and Interpreter to produce a list of commands
 * from a data source */
class StockListImporter {
  public:
    StockListImporter(Loader *loader, Parser *parser, Interpreter<StockListRecord> *interpreter):
      _loader ( loader ),
      _parser ( parser ),
      _interpreter ( interpreter ) {};

    StockListImporter (const StockListImporter&) = delete;
    StockListImporter& operator=(StockListImporter&) = delete;
    virtual ~StockListImporter() {};

    [[nodiscard]] CommandList<Catalogue, Inventory> import () const;

  private:
    std::unique_ptr<Loader> _loader;
    std::unique_ptr<Parser> _parser;
    std::unique_ptr<Interpreter<StockListRecord>> _interpreter;
};

class CSVFileInventoryImporter: public StockListImporter {
  public:
    CSVFileInventoryImporter (const std::filesystem::path &path) noexcept:
      StockListImporter (new FileLoader (path), new CSVParser (), new StockListInterpreter ()) {};
};

}  // namespace catapult

#endif  // __INVENTORY_IMPORTER_H__
