#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>

#include "stock_list_importer.h"

namespace catapult::testing {

using namespace ::testing;

TEST (TestStockListInterpreter, TestInterpret) {
  Record record { "", "MockProduct", "1.23", "MockGroup", "", "12" };
  const auto &[product, productGroup, quantity] = StockListInterpreter {}.interpret (record);
  EXPECT_EQ (Product ("MockProduct", 123), product);
  EXPECT_EQ (ProductGroup ("MockGroup"), productGroup);
  EXPECT_EQ (12, quantity);
}


class MockStockHolder: public MultiReceiver<Catalogue, Inventory> {
  public:
    MOCK_METHOD (void, addProduct, (const Product &), (override));
    MOCK_METHOD (void, addProductGroup, (const ProductGroup &), (override));
    MOCK_METHOD (void, addItems, (Product, uint32_t quantity), (override));
    MOCK_METHOD (uint32_t, getQuantity, (Product), (const override));
};

class UnderTestMultiReceiver {};

class MockLoader: public Loader {
  public:
    MOCK_METHOD (std::istream &, load, (), (const));
};

class MockParser: public Parser {
  public:
    MOCK_METHOD (std::vector<Record>, parse, (std::istream & data), (const));
};

class MockInterpreter: public Interpreter<StockListRecord> {
  public:
    MOCK_METHOD (StockListRecord, interpret, (const Record &record), (const));
};

TEST (TestStockListImporter, TestImport) {
  auto mockLoader = new MockLoader {};
  auto mockParser = new MockParser {};
  auto mockInterpreter = new MockInterpreter {};
  StockListImporter importer { mockLoader, mockParser, mockInterpreter };

  std::stringstream mockLoaderRetval { "Mock String" };
  EXPECT_CALL (*mockLoader, load ()).WillOnce (ReturnRef (mockLoaderRetval));
  EXPECT_CALL (*mockParser, parse).WillOnce (Return (std::vector<Record> { { "Hello!", "1.23", "21" }, { "Upstairs", "2.34", "12" } }));
  ON_CALL (*mockInterpreter, interpret)
    .WillByDefault (Return (std::make_tuple (Product ("hello", 123), ProductGroup ("MockGroup"), 12)));
  EXPECT_CALL (*mockInterpreter, interpret (Record { "Hello!", "1.23", "21" })).Times (1);
  EXPECT_CALL (*mockInterpreter, interpret (Record { "Upstairs", "2.34", "12" })).Times (1);

  std::ignore = importer.import ();
}

const std::string kTestData = R"T(0001,Apple,1.0,Fruit,THREE_FOR_TWO,100
0002,Banana,0.5,Fruit,THREE_IN_GROUP,150
0003,Milk,2.5,Dairy,THREE_FOR_TWO,200
)T";

class TestCSVFileInventoryImporter: public Test {
  public:
    void SetUp () override {
      _path = std::tmpnam (nullptr);
      std::ofstream tmpFile { _path };
      tmpFile << kTestData << std::flush;
    }

    void TearDown () override { std::filesystem::remove (_path); }

  protected:
    std::filesystem::path _path;
};

TEST_F (TestCSVFileInventoryImporter, TestCSVFileInventoryImporter) {
  MockStockHolder mockStockHolder {};
  EXPECT_CALL (mockStockHolder, addProduct (Product ("Apple", 100)));
  EXPECT_CALL (mockStockHolder, addProduct (Product ("Banana", 50)));
  EXPECT_CALL (mockStockHolder, addProduct (Product ("Milk", 250)));
  EXPECT_CALL (mockStockHolder, addProductGroup (ProductGroup ("Fruit")));
  EXPECT_CALL (mockStockHolder, addProductGroup (ProductGroup ("Dairy")));
  EXPECT_CALL (mockStockHolder, addItems (Product ("Apple", 100),100));
  EXPECT_CALL (mockStockHolder, addItems (Product ("Banana", 50),150));
  EXPECT_CALL (mockStockHolder, addItems (Product ("Milk", 250),200));

  CSVFileInventoryImporter underTest { _path };
  auto commandList = underTest.import ();
  mockStockHolder.applyCommandList (commandList);
}

}  // namespace catapult
