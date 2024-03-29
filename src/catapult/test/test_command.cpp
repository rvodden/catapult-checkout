#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "command.h"

namespace catapult::testing {

using namespace ::testing;

template<class Interface>
class MockUndoableCommand: public Undoable<Interface> {
  public:
    MOCK_METHOD (void, undo, (Interface & receiver), (const override));
};

class MockInterface {
  public:
    MockInterface (std::string name): _name { name } {};
    bool operator== (const MockInterface &other) const = default;

  private:
    std::string _name;
};

class MockBindableCommand: public BindableCommand<MockInterface> {
  public:
    MOCK_METHOD (void, _execute, (MockInterface & receiver), (const override));
};

TEST (TestBindableCommand, TestExecutionOfBoundCommand) {
  MockBindableCommand mockCommand;
  MockInterface interface {
      "Interface"
  };
  EXPECT_THAT (mockCommand.isBound (), IsFalse ());
  EXPECT_CALL (mockCommand, _execute (interface));
  mockCommand.bind (interface);
  EXPECT_THAT (mockCommand.isBound (), IsTrue ());
  mockCommand.execute (interface);
}

TEST (TestReverseCommand, TestExecute) {
  auto mockCommand = new MockUndoableCommand<uint8_t> {};
  ReverseCommand<uint8_t> underTest { mockCommand };
  uint8_t receiver = 8;
  EXPECT_CALL (*mockCommand, undo (receiver));
  underTest.execute (receiver);
}

class MockInterface1 {};

class MockCommand1: public Command<MockInterface1> {
  public:
    MOCK_METHOD (void, _execute, (MockInterface1 & receiver), (const override));
};


class MockInterface2 {};

class UnderTestMultiReceiver: public MultiReceiver<MockInterface1, MockInterface2> {};

class MockCommand2: public Command<MockInterface2> {
  public:
    MOCK_METHOD (void, _execute, (MockInterface2 & receiver), (const override));
};

TEST (TestMultiReceiver, TestExecuteCommandList) {
  auto mockCommand1 = std::make_shared<MockCommand1> ();
  auto mockCommand2 = std::make_shared<MockCommand2> ();
  CommandList<MockInterface1, MockInterface2> commandList { mockCommand1, mockCommand2 };
  UnderTestMultiReceiver underTest;
  EXPECT_CALL (*mockCommand1, _execute);
  EXPECT_CALL (*mockCommand2, _execute);
  underTest.applyCommandList (commandList);
}

TEST (TestMultiReceiver, TestExecuteCommandListAimedAtASubsetOfInterfaces) {
  auto mockCommand1 = std::make_shared<MockCommand1> ();
  CommandList<MockInterface1> commandList {
    mockCommand1,
  };
  UnderTestMultiReceiver underTest;
  EXPECT_CALL (*mockCommand1, _execute);
  underTest.applyCommandList (commandList);
}

class MockUndoableBindableCommand1: public UndoableBindableCommand<MockInterface1> {
  public:
    MOCK_METHOD (void, _execute, (MockInterface1 & interface), (const override));
    MOCK_METHOD (void, undo, (MockInterface1 & interface), (const override));
};

class MockUndoableBindableCommand2: public UndoableBindableCommand<MockInterface2> {
  public:
    MOCK_METHOD (void, _execute, (MockInterface2 & interface), (const override));
    MOCK_METHOD (void, undo, (MockInterface2 & interface), (const override));
};

TEST (TestTransact, TestAList) {
  auto mockUndoableBindableCommand1 = std::make_shared<MockUndoableBindableCommand1> ();
  auto mockUndoableBindableCommand2 = std::make_shared<MockUndoableBindableCommand2> ();
  MockInterface1 mockInterface1;
  MockInterface2 mockInterface2;

  EXPECT_CALL (*mockUndoableBindableCommand1, _execute);
  EXPECT_CALL (*mockUndoableBindableCommand2, _execute);
  Transaction<MockInterface1, MockInterface2> transaction {};
  transaction.then<MockInterface1> (mockInterface1, mockUndoableBindableCommand1)
    .then<MockInterface2> (mockInterface2, mockUndoableBindableCommand2)
    .execute ();
}

class MockUndoableBindableCommand3: public UndoableBindableCommand<MockInterface2> {
  public:
    MOCK_METHOD (void, _execute, (MockInterface2 & interface), (const override));
    MOCK_METHOD (void, undo, (MockInterface2 & interface), (const override));
};

TEST (TestTransact, TestAListIsRolledBackWhenOneThrows) {
  auto mockUndoableCommand1 = std::make_shared<MockUndoableBindableCommand1> ();
  auto mockUndoableCommand2 = std::make_shared<MockUndoableBindableCommand2> ();
  auto mockUndoableCommand3 = std::make_shared<MockUndoableBindableCommand3> ();
  MockInterface1 mockInterface1;
  MockInterface2 mockInterface2;

  EXPECT_CALL (*mockUndoableCommand1, _execute);
  EXPECT_CALL (*mockUndoableCommand2, _execute);
  EXPECT_CALL (*mockUndoableCommand3, _execute).WillRepeatedly(Throw(std::runtime_error("Any old error")));
  EXPECT_CALL (*mockUndoableCommand1, undo);
  EXPECT_CALL (*mockUndoableCommand2, undo);
  {
    Transaction<MockInterface1, MockInterface2> transaction {};
    transaction.then<MockInterface1> (mockInterface1, mockUndoableCommand1)
      .then<MockInterface2> (mockInterface2, mockUndoableCommand2)
      .then<MockInterface2> (mockInterface2, mockUndoableCommand3);
    EXPECT_THAT([&](){transaction.execute ();},Throws<std::runtime_error>() );
  }
}
}  // namespace catapult
