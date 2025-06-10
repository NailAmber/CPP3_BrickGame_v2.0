// src/brick_game/GameCommon.h
#ifndef S21_BRICK_GAME_COMMON_H
#define S21_BRICK_GAME_COMMON_H

#ifdef __cplusplus
namespace s21 {
extern "C" {  // To make C functions linkable from C++ if they were in a .c file
#endif

#include <stdbool.h>

// Board dimensions
const int FIELD_WIDTH = 10;
const int FIELD_HEIGHT = 20;

const int NEXT_FIELD_WIDTH = 4;
const int NEXT_FIELD_HEIGHT = 4;

// Enums for game elements on the field
enum CellState { EMPTY = 0, HEAD = 1, BODY = 2, FOOD = 3 };

// User actions
typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

// Game states for the Finite State Machine (FSM)
typedef enum {
  START_SCREEN,    // Before the game starts, showing instructions or initial
                   // screen
  GAME_RUNNING,    // Game is actively playing
  PAUSED,          // Game is temporarily halted by the user
  GAME_OVER_WIN,   // Game ended, player won
  GAME_OVER_LOSE,  // Game ended, player lost
  TERMINATE_GAME   // Game is exiting
} GameState;

// Game information structure passed to the GUI
typedef struct {
  int **field;  // The main game board (FIELD_HEIGHT x FIELD_WIDTH)
  int **next;  // Used for potential next piece (e.g., in Tetris), can be a stub
               // here for Snake
  int score;
  int high_score;
  int level;
  int speed;  // Represents the game update interval (lower value means faster)
  int pause;  // 0 for not paused, 1 for paused
  GameState current_game_state;
} GameInfo_t;

// Forward declarations for the game API functions
// These will be implemented in the s21::Game class
extern void userInput(UserAction_t action, bool hold);
extern GameInfo_t updateCurrentState();

#ifdef __cplusplus
}  // extern "C"
}  // namespace s21
#endif
#endif  // S21_BRICK_GAME_COMMON_H