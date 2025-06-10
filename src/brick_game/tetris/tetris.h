#ifndef S21_TETRIS_H
#define S21_TETRIS_H

#include "../GameCommon.h" // Assuming GameCommon.h is in src/brick_game/
#include <stdbool.h>     // For bool type in C
#include <stdio.h>       // For FILE operations (high score)

// --- Macros and Constants ---
#define TETRIS_BOARD_WIDTH 10     // From GameCommon.h (10)
#define TETRIS_BOARD_HEIGHT 20   // From GameCommon.h (20)
#define TETROMINO_GRID_SIZE 4              // Max size of a tetromino bounding box (e.g., 4x4)
#define NUM_TETROMINO_TYPES 7
#define NUM_TETROMINO_ROTATIONS 4
#define HIGH_SCORE_FILENAME "tetris_highscore.txt"

// --- Data Structures ---

// Structure to define a single tetromino shape and its dimensions within its 4x4 grid
typedef struct {
    int shape[TETROMINO_GRID_SIZE][TETROMINO_GRID_SIZE];
    // int width;  // Actual width of the piece - can be derived if needed
    // int height; // Actual height of the piece - can be derived if needed
} TetrominoShape;

// State of the current falling piece
typedef struct {
    int x;          // x-coordinate (column) of the top-left of the piece's 4x4 grid on the board
    int y;          // y-coordinate (row) of the top-left of the piece's 4x4 grid on the board
    int type;       // Index of the tetromino type (0-6)
    int rotation;   // Index of the current rotation (0-3)
    bool active;    // Is there a piece currently falling?
} CurrentPieceState;

// Internal Tetris FSM states (more granular than GameState from GameCommon.h)
typedef enum {
    TETRIS_STATE_START_SCREEN,  // Initial state, waiting for Start action
    TETRIS_STATE_SPAWN,         // Spawning a new piece
    TETRIS_STATE_MOVING,        // Piece is falling and can be controlled by player
    TETRIS_STATE_LOCKING,       // Piece has landed, locking it to the board
    TETRIS_STATE_LINE_CLEAR,    // Checking and clearing lines
    TETRIS_STATE_GAME_OVER      // Game over state
} TetrisFSMState_t;

// --- Game Logic API (to be called by the GUI) ---

/**
 * @brief Processes user input and updates the game state machine.
 *
 * This function is called by the GUI based on key presses.
 * It translates user actions into game commands like moving or rotating a piece,
 * pausing, or starting/terminating the game.
 *
 * @param action The user action (e.g., Left, Right, Rotate, Start, Pause).
 * @param hold Indicates if the action key is being held down (currently not fully utilized by CLI).
 */
void userInput(UserAction_t action, bool hold);

/**
 * @brief Updates the game state and returns all information needed for rendering.
 *
 * This function is called repeatedly by the GUI's game loop. It handles
 * automatic piece falling (gravity), checks for game events like landing a piece,
 * clearing lines, leveling up, and game over conditions.
 *
 * It allocates memory for 'field' and 'next' members of GameInfo_t.
 * The caller (GUI) is responsible for freeing this memory after use to prevent leaks.
 * Example:
 * GameInfo_t info = updateCurrentState();
 * // ... use info ...
 * for (int i = 0; i < FIELD_HEIGHT; ++i) free(info.field[i]);
 * free(info.field);
 * for (int i = 0; i < TETROMINO_GRID_SIZE; ++i) free(info.next[i]); // Assuming next is TETROMINO_GRID_SIZE height
 * free(info.next);
 *
 * @return GameInfo_t A structure containing the current game board, next piece,
 * score, high score, level, speed, and pause status.
 */
GameInfo_t updateCurrentState();


// --- Potentially useful internal functions that could be exposed if needed, ---
// --- but typically would be static in tetris.c                       ---

/**
 * @brief Initializes or resets the game to its starting state.
 * This is usually called internally when a 'Start' action is received
 * on the start screen or after a game over.
 */
void initialize_tetris_game(); // Made non-static for potential direct call if needed for testing/setup

#endif // S21_TETRIS_H