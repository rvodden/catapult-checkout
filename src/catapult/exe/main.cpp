#include <algorithm>
#include <cinttypes>
#include <iostream>

#include "catalogue.h"
#include "inventory_loader.h"
#include "super_market.h"

using namespace catapult;

int main ([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  CSVFileInventoryImporter importer { "seed.csv" };
  CommandList<Catalogue> commandList = importer.import ();

  SuperMarket superMarket;
  superMarket.applyCommandList(commandList);

  return 0;
}
