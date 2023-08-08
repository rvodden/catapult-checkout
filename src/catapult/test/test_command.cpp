#include "command.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace catapult::testing {

using namespace ::testing;

template<class Receiver>
class MockUndoableCommand: public Undoable<Receiver> {
  public:
    MOCK_METHOD (void, undo, (Receiver & receiver), (const override));
};

TEST (TestReverseCommand, ExecuteTest) {
  auto mockCommand = new MockUndoableCommand<uint8_t> {};
  ReverseCommand<uint8_t> underTest { mockCommand };
  uint8_t receiver = 8;
  EXPECT_CALL (*mockCommand, undo (receiver));
  underTest.execute (receiver);
}

class MockInterface1 {};
class MockInterface2 {};

class UnderTestMultiReceiver : public MultiReceiver<MockInterface1, MockInterface2> {};

class MockCommand1 : public Executable<MockInterface1> {
  public:
    MOCK_METHOD (void, execute, (MockInterface1& receiver), (const override));
};

class MockCommand2 : public Executable<MockInterface2> {

  public:
    MOCK_METHOD (void, execute, (MockInterface2& receiver), (const override));
};

TEST(TestMultiReceiver, TestExecuteCommandList) {
  auto mockCommand1 = std::make_shared<MockCommand1>();
  auto mockCommand2 = std::make_shared<MockCommand2>();
  CommandList<MockInterface1, MockInterface2> commandList {
    mockCommand1,
    mockCommand2
  };
  UnderTestMultiReceiver underTest;
  EXPECT_CALL(*mockCommand1, execute);
  EXPECT_CALL(*mockCommand2, execute);
  underTest.applyCommandList(commandList);
}

TEST(TestMultiReceiver, TestExecuteCommandListAimedAtASubsetOfInterfaces) {
  auto mockCommand1 = std::make_shared<MockCommand1>();
  CommandList<MockInterface1> commandList {
    mockCommand1,
  };
  UnderTestMultiReceiver underTest;
  EXPECT_CALL(*mockCommand1, execute);
  underTest.applyCommandList(commandList);
}

}  // namespace catapult
