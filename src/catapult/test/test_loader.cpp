#include "loader.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string_view>
#include <vector>

namespace catapult::testing {

using namespace ::testing;
using namespace std::literals;

using CSVTestDataType = std::pair<std::string, std::vector<std::vector<std::string>>>;

class CSVTest: public ::testing::TestWithParam<CSVTestDataType> {
    virtual void SetUp () override {
      auto param = GetParam ();
      std::tie (input, expected) = param;
    }

  protected:
    std::string input;
    std::vector<std::vector<std::string>> expected;
};

const auto csvData = Values (
  CSVTestDataType { "value1,value2\n", { { "value1", "value2" } } },
  CSVTestDataType { "value1\n", { { "value1" } } },
  CSVTestDataType { "\n", { {} } });

TEST_P (CSVTest, TestParsing) {
  CSVParser underTest {};
  std::stringstream stringStream { input };
  auto result = underTest.parse (stringStream);
  EXPECT_THAT (std::size (result), Eq (std::size (expected)));

  auto resultantLineIterator = result.cbegin ();
  auto expectedLineIterator = expected.cbegin ();
  for (; resultantLineIterator != result.end () && expectedLineIterator != expected.end ();
       resultantLineIterator++, expectedLineIterator++) {
    EXPECT_THAT (*resultantLineIterator, ElementsAreArray (*expectedLineIterator));
  };
}

INSTANTIATE_TEST_SUITE_P (TestCSVParser, CSVTest, csvData);

constexpr auto kTestData = "This is some test data\n"sv;

class TestFileLoader: public Test {
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

TEST_F (TestFileLoader, FileTest) {
  FileLoader underTest { _path };
  std::stringstream sStream;
  sStream << underTest.load ().rdbuf ();
  EXPECT_EQ (sStream.str (), kTestData);
}

class UnderTestReceiver {};

class MockLoader: public Loader {
  public:
    MOCK_METHOD (std::istream &, load, (), (const));
};

class MockParser: public Parser {
  public:
    MOCK_METHOD (std::vector<Record>, parse, (std::istream & data), (const));
};

class MockInterpreter: public Interpreter<UnderTestReceiver> {
  public:
    MOCK_METHOD (CommandList<UnderTestReceiver>, interpret, (const Record &record), (const));
};

TEST (TestImporter, ImporterTest) {
  auto mockLoader = new MockLoader {};
  auto mockParser = new MockParser {};
  auto mockInterpreter = new MockInterpreter {};
  Importer<UnderTestReceiver> importer { mockLoader, mockParser, mockInterpreter };

  std::stringstream mockLoaderRetval { "Mock String" };
  EXPECT_CALL (*mockLoader, load ()).WillOnce (ReturnRef ( mockLoaderRetval ));
  EXPECT_CALL (*mockParser, parse).WillOnce(Return( std::vector<Record> { {"Hello!", "GoodBye!"}, { "Upstairs", "Downstairs"} } ));
  EXPECT_CALL (*mockInterpreter, interpret( Record{"Hello!", "GoodBye!"})).Times(1);
  EXPECT_CALL (*mockInterpreter, interpret( Record{"Upstairs", "Downstairs"})).Times(1);

  std::ignore = importer.import();
}

}  // namespace catapult
