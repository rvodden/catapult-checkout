#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>

#include "inventory_importer.h"

namespace catapult {

using namespace ::testing;

TEST (TestInventoryInterpreter, InterpreterTest) {
  Record record { "", "MockProduct", "1.23", "MockGroup", "", "" };
  const auto &[product, productGroup] = InventoryInterpreter {}.interpret (record);
  EXPECT_EQ(Product("MockProduct", 123), product);
  EXPECT_EQ(ProductGroup("MockGroup"), productGroup);
}


class MockCatalogue: public Catalogue {
  public:
    MOCK_METHOD (void, addProduct, (const Product &), (override));
    MOCK_METHOD (void, addProductGroup, (const ProductGroup &), (override));
};

class UnderTestReceiver {};

class MockLoader: public Loader {
  public:
    MOCK_METHOD (std::istream &, load, (), (const));
};

class MockParser: public Parser {
  public:
    MOCK_METHOD (std::vector<Record>, parse, (std::istream & data), (const));
};

class MockInterpreter: public Interpreter<InterpretedRecord> {
  public:
    MOCK_METHOD (InterpretedRecord, interpret, (const Record &record), (const));
};

TEST (TestInventoryImporter, ImporterTest) {
  auto mockLoader = new MockLoader {};
  auto mockParser = new MockParser {};
  auto mockInterpreter = new MockInterpreter {};
  InventoryImporter importer { mockLoader, mockParser, mockInterpreter };

  std::stringstream mockLoaderRetval { "Mock String" };
  EXPECT_CALL (*mockLoader, load ()).WillOnce (ReturnRef ( mockLoaderRetval ));
  EXPECT_CALL (*mockParser, parse).WillOnce(Return( std::vector<Record> { {"Hello!", "1.23"}, { "Upstairs", "2.34"} } ));
  ON_CALL (*mockInterpreter, interpret).WillByDefault(Return(std::make_tuple(Product("hello", 123), ProductGroup("MockGroup"))));
  EXPECT_CALL (*mockInterpreter, interpret( Record{"Hello!", "1.23"})).Times(1);
  EXPECT_CALL (*mockInterpreter, interpret( Record{"Upstairs", "2.34"})).Times(1);

  std::ignore = importer.import();
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
  MockCatalogue mockCatalogue {};
  EXPECT_CALL (mockCatalogue, addProduct (Product ("Apple", 100)));
  EXPECT_CALL (mockCatalogue, addProduct (Product ("Banana", 50)));
  EXPECT_CALL (mockCatalogue, addProduct (Product ("Milk", 250)));
  EXPECT_CALL (mockCatalogue, addProductGroup (ProductGroup ("Fruit"))).Times(2);
  EXPECT_CALL (mockCatalogue, addProductGroup (ProductGroup ("Dairy")));

  CSVFileInventoryImporter underTest { _path };
  auto commandList = underTest.import ();
  mockCatalogue.applyCommandList(commandList);
}

}  // namespace catapult
