#include <algorithm>
#include <cinttypes>
#include <iostream>
#include <filesystem>

#include "exception.h"
#include "stock_list_importer.h"
#include "super_market_renderer.h"
#include "super_market.h"

using namespace catapult;

int main ([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  try {
    std::cout << std::filesystem::current_path() << std::endl;
    CSVFileInventoryImporter importer { "/workspaces/catapult-checkout/build/src/catapult/seed.csv" };
    auto commandList = importer.import ();

    SuperMarket superMarket;
    superMarket.applyCommandList (commandList);

    Basket basket;

    Product banana = superMarket.getProductByName("Banana");
    auto buyThreeBananas = Inventory::MoveStockCommandFactory::build(superMarket, basket, banana, 3);
    buyThreeBananas.execute();

    TextRenderer textRenderer;
    textRenderer.render(basket);

  } catch ( const CannotOpenFileException& cofe ) {
    std::cerr << "Cannot open ../seed.csv" << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
