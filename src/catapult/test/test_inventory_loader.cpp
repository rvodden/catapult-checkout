#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>

#include "inventory_loader.h"

namespace catapult {

using namespace ::testing;

class MockCatalogue: public Catalogue {
  public:
    MOCK_METHOD (void, addProduct, (const Product &), (override));
};

TEST (TestInventoryInterpreter, InterpreterTest) {
  Record record { "", "MockProduct", "1.23", "", "", "" };
  CommandList<Catalogue> commandList = InventoryInterpreter {}.interpret (record);
  MockCatalogue mockCatalogue {};
  EXPECT_CALL (mockCatalogue, addProduct (Product ("MockProduct", 123)));
  for (const auto &command: commandList) {
    command->execute (mockCatalogue);
  };
}

const std::string kTestData = R"T(0001,Apple,1.0,FRUIT,THREE_FOR_TWO,100
0002,Banana,0.5,FRUIT,THREE_IN_GROUP,150
0003,Milk,2.5,DAIRY,THREE_FOR_TWO,200
)T";

class TestInventoryImporter: public Test {
  public:
    void SetUp () {
      _path = std::tmpnam (nullptr);
      std::ofstream tmpFile { _path };
      tmpFile << kTestData << std::flush;
    }

    void TearDown () { std::filesystem::remove (_path); }

  protected:
    std::filesystem::path _path;
};

TEST_F (TestInventoryImporter, TestCSVFileInventoryImporter) {
  MockCatalogue mockCatalogue {};
  EXPECT_CALL (mockCatalogue, addProduct (Product ("Apple", 100)));
  EXPECT_CALL (mockCatalogue, addProduct (Product ("Banana", 50)));
  EXPECT_CALL (mockCatalogue, addProduct (Product ("Milk", 250)));

  CSVFileInventoryImporter underTest { _path };
  auto commandList = underTest.import ();
  mockCatalogue.applyCommandList(commandList);
}

}  // namespace catapult
