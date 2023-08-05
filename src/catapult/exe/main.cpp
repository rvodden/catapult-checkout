#include <algorithm>
#include <cinttypes>
#include <iostream>

#include "super_market.h"
#include "stock_list_importer.h"

using namespace catapult;

int main ([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  CSVFileInventoryImporter importer { "seed.csv" };
  auto commandList = importer.import ();

  SuperMarket superMarket;
  superMarket.applyCommandList (commandList);

  return 0;
}
