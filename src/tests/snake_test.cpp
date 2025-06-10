#include "../brick_game/snake/snake.h"

#include <gtest/gtest.h>

using namespace s21;

// Test fixture for the Snake game
class SnakeGameTest : public ::testing::Test {
 protected:
  // You can define per-test set-up and tear-down logic as
  // virtual void SetUp() and virtual void TearDown() methods.
  void SetUp() override {
    // Reset the game instance before each test
    Game& game = Game::getInstance();
    game.resetGame();
  }

  // Helper to deallocate memory within a GameInfo_t struct
  void destroyGameInfo(GameInfo_t& info) {
    if (info.field != nullptr) {
      for (int i = 0; i < FIELD_HEIGHT; ++i) {  // FIELD_HEIGHT from snake.h
        delete[] info.field[i];
      }
      delete[] info.field;
      info.field = nullptr;
    }
    if (info.next != nullptr) {
      for (int i = 0; i < 4; ++i) {  // Assuming next_field_ is 4x4
        delete[] info.next[i];
      }
      delete[] info.next;
      info.next = nullptr;
    }
  }
};

// Test case for initial game state
TEST_F(SnakeGameTest, InitialState) {
  GameInfo_t state = updateCurrentState();
  EXPECT_EQ(state.current_game_state, START_SCREEN);
  EXPECT_EQ(state.score, 0);
  EXPECT_EQ(state.level, 1);
  EXPECT_EQ(state.speed, 500);
  EXPECT_FALSE(state.pause);
  destroyGameInfo(state);
}

// Test case for starting the game
TEST_F(SnakeGameTest, StartGame) {
  userInput(Start, false);
  GameInfo_t state = updateCurrentState();
  EXPECT_EQ(state.current_game_state, GAME_RUNNING);
  destroyGameInfo(state);
}

// Test case for pausing and unpausing the game
TEST_F(SnakeGameTest, PauseAndUnpause) {
  // Start the game first
  userInput(Start, false);
  GameInfo_t state = updateCurrentState();
  EXPECT_FALSE(state.pause);
  destroyGameInfo(state);

  // Pause the game
  userInput(Pause, false);
  state = updateCurrentState();
  EXPECT_EQ(state.current_game_state, PAUSED);
  EXPECT_TRUE(state.pause);
  destroyGameInfo(state);

  // Unpause the game
  userInput(Pause, false);
  state = updateCurrentState();
  EXPECT_EQ(state.current_game_state, GAME_RUNNING);
  EXPECT_FALSE(state.pause);
  destroyGameInfo(state);
}

// Test case for snake movement
TEST_F(SnakeGameTest, SnakeMovement) {
  userInput(Start, false);  // Start the game
  GameInfo_t state = updateCurrentState();
  EXPECT_EQ(state.current_game_state, GAME_RUNNING);

  // Get initial snake position
  GameInfo_t initialState = updateCurrentState();
  int initialHeadX = -1, initialHeadY = -1;
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      if (initialState.field[i][j] == HEAD) {
        initialHeadX = j;
        initialHeadY = i;
        break;
      }
    }
    if (initialHeadX != -1) break;
  }

  // Let the game run for one tick
  GameInfo_t stateAfterMove = updateCurrentState();

  // Find the new head position
  int newHeadX = -1, newHeadY = -1;
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      if (stateAfterMove.field[i][j] == HEAD) {
        newHeadX = j;
        newHeadY = i;
        break;
      }
    }
    if (newHeadX != -1) break;
  }

  // Snake starts by moving right
  EXPECT_EQ(newHeadX, initialHeadX + 1);
  EXPECT_EQ(newHeadY, initialHeadY);
  destroyGameInfo(state);
  destroyGameInfo(stateAfterMove);
  destroyGameInfo(initialState);
}

// Test case for game over by hitting a wall
TEST_F(SnakeGameTest, GameOverWallCollision) {
  userInput(Start, false);  // Start the game

  // Move the snake to the wall
  // This requires many steps, so we'll just run the game loop
  GameInfo_t state;
  for (int i = 0; i < FIELD_WIDTH; ++i) {
    state = updateCurrentState();
    if (state.current_game_state == GAME_OVER_LOSE) {
      break;
    }
    destroyGameInfo(state);
  }
  destroyGameInfo(state);

  GameInfo_t finalState = updateCurrentState();
  EXPECT_EQ(finalState.current_game_state, GAME_OVER_LOSE);
  destroyGameInfo(finalState);

  userInput(Start, false);
  finalState = updateCurrentState();
  EXPECT_EQ(finalState.current_game_state, START_SCREEN);
  destroyGameInfo(finalState);
}

// Test case for not game over by self-collision
TEST_F(SnakeGameTest, GameOverSelfCollision) {
  userInput(Start, false);  // Start the game

  // A sequence of moves to no cause self-collision
  // Right (initial) -> Down -> Left -> Up
  GameInfo_t state = updateCurrentState();  // Move Right
  userInput(Right, false);                  // Turn Down
  destroyGameInfo(state);
  state = updateCurrentState();  // Move Down
  destroyGameInfo(state);
  userInput(Right, false);       // Turn Left
  state = updateCurrentState();  // Move Left
  userInput(Right, false);       // Turn Up -> no collision
  destroyGameInfo(state);

  GameInfo_t finalState = updateCurrentState();
  EXPECT_EQ(finalState.current_game_state, GAME_RUNNING);
  destroyGameInfo(finalState);
}

// Test case for resetting the game
TEST_F(SnakeGameTest, ResetGame) {
  userInput(Start, false);                  // Start
  GameInfo_t state = updateCurrentState();  // Move snake
  EXPECT_EQ(state.current_game_state, GAME_RUNNING);
  destroyGameInfo(state);
  userInput(Pause, false);  // Pause

  // Now reset from the paused state
  userInput(Start, false);  // This should reset the game

  state = updateCurrentState();
  EXPECT_EQ(state.current_game_state, START_SCREEN);
  EXPECT_EQ(state.score, 0);
  EXPECT_EQ(state.level, 1);
  destroyGameInfo(state);
}

// Test terminating the game
TEST_F(SnakeGameTest, TerminateGame) {
  userInput(Terminate, false);
  GameInfo_t state = updateCurrentState();
  EXPECT_EQ(state.current_game_state, TERMINATE_GAME);
  destroyGameInfo(state);

  // From PAUSED state
  SetUp();  // Reset
  userInput(Start, false);
  userInput(Pause, false);
  userInput(Terminate, false);
  state = updateCurrentState();
  EXPECT_EQ(state.current_game_state, TERMINATE_GAME);
  destroyGameInfo(state);

  SetUp();  // Reset
  userInput(Start, false);
  userInput(Terminate, false);
  state = updateCurrentState();
  EXPECT_EQ(state.current_game_state, TERMINATE_GAME);
  destroyGameInfo(state);
}

// Test terminating the game
TEST_F(SnakeGameTest, SpeedUp) {
  userInput(Start, false);
  userInput(Action, false);
  userInput(Action, false);
  userInput(Action, false);
  userInput(Action, false);
  userInput(Action, false);
  GameInfo_t state = updateCurrentState();
  EXPECT_EQ(state.current_game_state, GAME_OVER_LOSE);
  destroyGameInfo(state);

  userInput(Terminate, false);
  state = updateCurrentState();
  EXPECT_EQ(state.current_game_state, TERMINATE_GAME);
  destroyGameInfo(state);
}

// Test terminating the game
TEST_F(SnakeGameTest, EatApple) {
  for (int i = 0; i < 1000; ++i) {
    SetUp();
    userInput(Start, false);
    userInput(Action, false);
    userInput(Left, false);
    userInput(Action, false);
    userInput(Action, false);
    userInput(Action, false);
  }
}

// Main function for running the tests
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}