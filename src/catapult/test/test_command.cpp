#include "command.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace catapult::testing {

using namespace ::testing;

template<class Interface>
class MockUndoableCommand: public Undoable<Interface> {
  public:
    MOCK_METHOD (void, undo, (Interface & receiver), (const override));
};

class MockInterface {
  public:
    MockInterface(std::string name): _name{name} {};
    bool operator==(const MockInterface& other) const = default;

  private:
    std::string _name;
};

class MockCommand : public Command<MockInterface> {
  public:
    MOCK_METHOD (void, _execute, (MockInterface& receiver), (const override));
};

TEST (TestCommand, TestExecutionOfBoundCommand) {
  MockCommand mockCommand;
  MockInterface interface { "Interface" };
  EXPECT_THAT(mockCommand.isBound(), IsFalse());
  EXPECT_CALL(mockCommand, _execute(interface));
  mockCommand.bind(interface);
  EXPECT_THAT(mockCommand.isBound(), IsTrue());
  mockCommand.execute(interface);
}

TEST (TestCommand, TestExecuteBindsCommand) {
  MockCommand mockCommand;
  MockInterface interface { "Interface" };
  EXPECT_THAT(mockCommand.isBound(), IsFalse());
  mockCommand.execute(interface);
  EXPECT_THAT(mockCommand.isBound(), IsTrue());
}

TEST (TestReverseCommand, TestExecute) {
  auto mockCommand = new MockUndoableCommand<uint8_t> {};
  ReverseCommand<uint8_t> underTest { mockCommand };
  uint8_t receiver = 8;
  EXPECT_CALL (*mockCommand, undo (receiver));
  underTest.execute (receiver);
}

class MockInterface1 { };

class MockCommand1 : public Command<MockInterface1> {
  public:
    MOCK_METHOD (void, _execute, (MockInterface1& receiver), (const override));
};


class MockInterface2 {};

class UnderTestMultiReceiver : public MultiReceiver<MockInterface1, MockInterface2> {};

class MockCommand2 : public Command<MockInterface2> {

  public:
    MOCK_METHOD (void, _execute, (MockInterface2& receiver), (const override));
};

TEST(TestMultiReceiver, TestExecuteCommandList) {
  auto mockCommand1 = std::make_shared<MockCommand1>();
  auto mockCommand2 = std::make_shared<MockCommand2>();
  CommandList<MockInterface1, MockInterface2> commandList {
    mockCommand1,
    mockCommand2
  };
  UnderTestMultiReceiver underTest;
  EXPECT_CALL(*mockCommand1, _execute);
  EXPECT_CALL(*mockCommand2, _execute);
  underTest.applyCommandList(commandList);
}

TEST(TestMultiReceiver, TestExecuteCommandListAimedAtASubsetOfInterfaces) {
  auto mockCommand1 = std::make_shared<MockCommand1>();
  CommandList<MockInterface1> commandList {
    mockCommand1,
  };
  UnderTestMultiReceiver underTest;
  EXPECT_CALL(*mockCommand1, _execute);
  underTest.applyCommandList(commandList);
}

// TEST(TestTransact, TestAList) {
//   auto mockCommand1 = std::make_shared<MockCommand1>();
//   auto mockCommand2 = std::make_shared<MockCommand2>();
//   MockInterface1 mockInterface1;
//   MockInterface2 mockInterface2;

//   CommandList<MockInterface1, MockInterface2> commandList {
//     mockCommand1,
//     mockCommand2
//   };
//   EXPECT_CALL(*mockCommand1, _execute);
//   EXPECT_CALL(*mockCommand2, _execute);
//   transact(commandList, mockInterface1, mockInterface2);

// }

}  // namespace catapult
