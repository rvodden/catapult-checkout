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

class UnderTestReceiver : public Receiver<UnderTestReceiver> {
};

class MockCommand : public Executable<UnderTestReceiver> {
  public:
    using Executable<UnderTestReceiver>::execute;
    MOCK_METHOD (void, execute, (UnderTestReceiver& receiver), (const override));
};

TEST(TestReceiver, TestExecuteCommandList) {
  auto mockCommand1 = std::make_shared<MockCommand>();
  auto mockCommand2 = std::make_shared<MockCommand>();
  CommandList<UnderTestReceiver> commandList {
    mockCommand1,
    mockCommand2
  };
  UnderTestReceiver underTest;
  EXPECT_CALL(*mockCommand1, execute);
  EXPECT_CALL(*mockCommand2, execute);
  underTest.applyCommandList(commandList);
}

}  // namespace catapult
