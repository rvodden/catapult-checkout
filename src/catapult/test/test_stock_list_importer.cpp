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
    MOCK_METHOD (void, _addProduct, (const Product &), (override));
    MOCK_METHOD (void, _addProductGroup, (const ProductGroup &), (override));
    MOCK_METHOD (void, _addProductToGroup, (const Product&, const ProductGroup &), (override));
    MOCK_METHOD (Product, getProductByName, (const std::string &), (const override));
    MOCK_METHOD (std::vector<std::reference_wrapper<const Product>>, getProductsInProductGroup, (const ProductGroup &), (const override));
    MOCK_METHOD (void, _addItems, (Product, uint32_t quantity), (override));
    MOCK_METHOD (void, _removeItems, (Product, uint32_t quantity), (override));
    MOCK_METHOD (uint32_t, getQuantity, (Product), (const override));
    MOCK_METHOD (std::vector<Product>, getProducts, (), (const override4));
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
      char filename[] = "/tmp/mytemp.XXXXXX";
      int fileDescriptor = mkstemp(filename);
      close(fileDescriptor);
      _path = std::filesystem::path(filename);
      std::ofstream tmpFile { _path };
      tmpFile << kTestData << std::flush;
    }

    void TearDown () override { std::filesystem::remove (_path); }

  protected:
    std::filesystem::path _path;
};

TEST_F (TestCSVFileInventoryImporter, TestCSVFileInventoryImporter) {
  MockStockHolder mockStockHolder {};
  auto apple = Product("Apple", 100);
  auto banana = Product("Banana", 50);
  auto milk = Product("Milk", 250);

  auto fruit = ProductGroup("Fruit");
  auto dairy = ProductGroup("Dairy");
  EXPECT_CALL (mockStockHolder, _addProduct (apple));
  EXPECT_CALL (mockStockHolder, _addProduct (banana));
  EXPECT_CALL (mockStockHolder, _addProduct (milk));
  EXPECT_CALL (mockStockHolder, _addProductGroup (fruit));
  EXPECT_CALL (mockStockHolder, _addProductGroup (dairy));
  EXPECT_CALL (mockStockHolder, _addItems (apple,100));
  EXPECT_CALL (mockStockHolder, _addItems (banana,150));
  EXPECT_CALL (mockStockHolder, _addItems (milk,200));
  EXPECT_CALL (mockStockHolder, _addProductToGroup (apple, fruit));
  EXPECT_CALL (mockStockHolder, _addProductToGroup (banana, fruit));
  EXPECT_CALL (mockStockHolder, _addProductToGroup (milk, dairy));

  CSVFileInventoryImporter underTest { _path };
  auto commandList = underTest.import ();
  mockStockHolder.applyCommandList (commandList);
}

}  // namespace catapult
