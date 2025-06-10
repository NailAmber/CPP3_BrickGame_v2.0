#ifndef S21_BRICK_GAME_SNAKE_GAME_H
#define S21_BRICK_GAME_SNAKE_GAME_H

#include <deque>   // For snake body segments
#include <random>  // For random food generation
#include <string>  // For high score file path

#include "../GameCommon.h"  // Include common definitions

namespace s21 {

/**
 * @brief Forward declaration of the Game class.
 */
class Game;

/**
 * @brief Handles user input for the Snake game.
 *
 * @param action The user action (direction, pause, etc).
 * @param hold Whether the action is being held down.
 */
void userInput(UserAction_t action, bool hold);

/**
 * @brief Updates and retrieves the current state of the Snake game.
 *
 * @return GameInfo_t The current game state information.
 */
GameInfo_t updateCurrentState();

/**
 * @brief Represents a point (x, y) on the game field.
 */
struct Point {
  int x;  ///< X coordinate.
  int y;  ///< Y coordinate.

  /**
   * @brief Equality operator for Point.
   * @param other The point to compare with.
   * @return true if points are equal, false otherwise.
   */
  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }
  /**
   * @brief Adds two points (vector addition).
   * @param other The point to add.
   * @return The resulting point.
   */
  Point operator+(const Point& other) const {
    return {x + other.x, y + other.y};
  }
};

/**
 * @brief The main Snake game logic and state manager (Singleton).
 *
 * This class encapsulates all game logic, state, and data for the Snake game.
 * It uses the singleton pattern to ensure only one instance exists.
 */
class Game {
 public:
  /**
   * @brief Retrieves the singleton instance of the Snake game.
   * @return Reference to the Game instance.
   */
  static Game& getInstance();

  // Delete copy constructor and assignment operator for singleton
  Game(const Game&) = delete;
  Game& operator=(const Game&) = delete;

  /**
   * @brief Handles user input and updates internal state accordingly.
   * @param action The user action (direction, pause, etc).
   * @param hold Whether the action is being held down.
   */
  void handleUserInput(UserAction_t action, bool hold);

  /**
   * @brief Retrieves the current state of the game.
   * @return GameInfo_t The current game state information.
   */
  GameInfo_t getCurrentState();

  /**
   * @brief Resets the game to its initial state.
   */
  void resetGame();

 private:
  /**
   * @brief Private constructor for singleton pattern.
   */
  Game();

  /**
   * @brief Destructor for cleanup.
   */
  ~Game();

  // Game state
  GameState current_state_;  ///< Current finite state machine state.

  // Game data
  int** game_field_;         ///< 2D array representing the game field.
  int** next_field_;         ///< 2D array for next state calculations.
  std::deque<Point> snake_;  ///< Snake body segments (head at front).
  Point food_position_;      ///< Current food position.
  int score_;                ///< Current score.
  int high_score_;           ///< Highest score achieved (persistent).
  int level_;                ///< Current game level.
  int speed_;  ///< Update interval in milliseconds (lower = faster).
  // int tick_counter_; // Not strictly needed if QTimer directly uses speed_

  Point snake_direction_;  ///< Current movement direction of the snake.

  // Private helper functions for game logic

  /**
   * @brief Initializes or resets the game state and data.
   */
  void initializeGame();

  /**
   * @brief Generates a new food position on the field.
   */
  void generateFood();

  /**
   * @brief Moves the snake in the current direction.
   */
  void moveSnake();

  /**
   * @brief Increases the snake's speed (decreases update interval).
   */
  void increaseSnakeSpeed();

  /**
   * @brief Loads the high score from persistent storage.
   */
  void loadHighScore();

  /**
   * @brief Saves the current high score to persistent storage.
   */
  void saveHighScore();

  /**
   * @brief Updates the game logic (FSM step).
   */
  void updateGameLogic();

  /**
   * @brief File path for storing the high score.
   */
  const std::string kHighScoreFilePath = "high_score.txt";
};

}  // namespace s21

#endif  // S21_BRICK_GAME_SNAKE_GAME_H