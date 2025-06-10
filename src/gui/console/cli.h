#ifndef S21_BRICKGAME_CLI_H
#define S21_BRICKGAME_CLI_H

#include <ncurses.h>  // For ncurses functions

#include <chrono>  // For std::chrono::milliseconds
#include <string>  // For std::to_string
#include <thread>  // For std::this_thread::sleep_for

// Assuming GameController.h defines the s21 namespace, GameInfo_t, constants,
// etc.
#include "../../brick_game/GameController.h"

// Namespace alias for convenience
namespace game = s21;

// Draws the current game state to the ncurses console.
void draw_game(const game::GameInfo_t& game_info);

#endif  // S21_BRICKGAME_CLI_H