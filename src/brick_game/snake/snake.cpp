#include "snake.h"

#include <algorithm>  // For std::max, std::any_of
#include <chrono>     // For random seed
#include <fstream>    // For file I/O for high score

namespace s21 {

// --- Global API Functions (as per specification) ---

void userInput(UserAction_t action, bool hold) {
  (void)hold;
  Game::getInstance().handleUserInput(action, hold);
}

GameInfo_t updateCurrentState() {
  return Game::getInstance().getCurrentState();
}

// --- Game Class Implementation ---

Game& Game::getInstance() {
  static Game instance;
  return instance;
}

Game::Game()
    : current_state_(START_SCREEN),
      score_(0),
      high_score_(0),
      level_(1),
      speed_(500),  // Initial speed: 500 ms update interval
      snake_direction_({1, 0}) {
  // Initialize random seed
  srand(static_cast<unsigned int>(
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count()));

  loadHighScore();   // Load high score on game initialization
  initializeGame();  // Set up initial game state
}

Game::~Game() {
  saveHighScore();  // Save high score on game exit

  // Remember to deallocate memory when done
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    delete[] game_field_[i];
  }
  delete[] game_field_;

  for (int i = 0; i < 4; ++i) {
    delete[] next_field_[i];
  }
  delete[] next_field_;
}

void Game::initializeGame() {
  // Deallocate existing memory if it's already allocated (e.g., on reset)
  // Check if game_field_ is not nullptr before attempting to delete
  if (game_field_ != nullptr) {
    for (int i = 0; i < FIELD_HEIGHT; ++i) {
      delete[] game_field_[i];
    }
    delete[] game_field_;
    game_field_ = nullptr;  // Set to nullptr after deallocation
  }

  if (next_field_ != nullptr) {
    for (int i = 0; i < 4; ++i) {
      delete[] next_field_[i];
    }
    delete[] next_field_;
    next_field_ = nullptr;  // Set to nullptr after deallocation
  }

  // Initialize the game field with empty cells
  game_field_ = new int*[FIELD_HEIGHT];

  // Allocate memory for each row (FIELD_WIDTH)
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    game_field_[i] = new int[FIELD_WIDTH];
  }

  // Initialize the elements (e.g., to 0)
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      game_field_[i][j] = 0;  // Initialize each element
    }
  }

  next_field_ = new int*[4];

  // Allocate memory for each row (4)
  for (int i = 0; i < 4; ++i) {
    next_field_[i] = new int[4];
  }

  // Initialize the elements (e.g., to 0)
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      next_field_[i][j] = 0;  // Initialize each element
    }
  }

  // Initialize snake (4 segments, starting horizontally near the center)
  snake_.clear();
  // Snake starts moving right, so its body segments should be to its left
  snake_.push_back({FIELD_WIDTH / 2, FIELD_HEIGHT / 2});  // Head
  snake_.push_back({FIELD_WIDTH / 2 - 1, FIELD_HEIGHT / 2});
  snake_.push_back({FIELD_WIDTH / 2 - 2, FIELD_HEIGHT / 2});
  snake_.push_back({FIELD_WIDTH / 2 - 3, FIELD_HEIGHT / 2});

  // Reset initial direction to right
  snake_direction_ = {1, 0};

  // Place snake on the field
  game_field_[snake_.front().y][snake_.front().x] = HEAD;
  for (size_t i = 1; i < snake_.size(); ++i) {
    game_field_[snake_[i].y][snake_[i].x] = BODY;
  }

  generateFood();  // Place initial food
}

void Game::resetGame() {
  score_ = 0;
  level_ = 1;
  speed_ = 500;                   // Reset to initial speed
  initializeGame();               // This will also reset snake_direction_
  current_state_ = START_SCREEN;  // Go back to start screen after reset
}

void Game::generateFood() {
  bool placed = false;
  while (!placed) {
    int food_x, food_y;
    food_x = rand() % FIELD_WIDTH;
    food_y = rand() % FIELD_HEIGHT;

    // Check if the random position is not occupied by the snake
    bool occupied =
        std::any_of(snake_.begin(), snake_.end(), [&](const Point& segment) {
          return segment.x == food_x && segment.y == food_y;
        });

    if (!occupied) {
      food_position_ = {food_x, food_y};
      game_field_[food_y][food_x] = FOOD;
      placed = true;
    }
  }
}

void Game::moveSnake() {
  // Check if food is eaten first, as this might change the tail behavior
  Point old_head = snake_.front();
  Point new_head = old_head + snake_direction_;  // Use overloaded + operator

  // Check for collision before moving the snake
  // Wall collision
  if (new_head.x < 0 || new_head.x >= FIELD_WIDTH || new_head.y < 0 ||
      new_head.y >= FIELD_HEIGHT) {
    current_state_ = GAME_OVER_LOSE;
    return;
  }

  // Self-collision (check new head against existing body, excluding potential
  // tail) If food is eaten, the tail doesn't move, so new head might collide
  // with old tail. If not, the tail moves, so new head shouldn't collide with
  // the space the tail just vacated.
  bool food_eaten =
      (new_head.x == food_position_.x && new_head.y == food_position_.y);

  // Iterate over snake body, but exclude the very last segment if food is NOT
  // eaten because that segment will be moved.
  for (size_t i = 0; i < snake_.size() - (food_eaten ? 0 : 1); ++i) {
    if (new_head == snake_[i]) {  // Using overloaded == operator
      current_state_ = GAME_OVER_LOSE;
      return;
    }
  }

  // Update game field and snake body
  // Clear the old tail's position *if* it's moving
  if (!food_eaten) {
    Point tail = snake_.back();
    game_field_[tail.y][tail.x] = EMPTY;
    snake_.pop_back();
  }

  // Add new head
  snake_.push_front(new_head);
  game_field_[new_head.y][new_head.x] = HEAD;
  game_field_[old_head.y][old_head.x] = BODY;  // Old head becomes body

  if (food_eaten) {
    // Food eaten: increase score, generate new food, potentially level up
    score_++;
    if (score_ > high_score_) {
      high_score_ = score_;
    }
    if (score_ % 5 == 0 &&
        level_ < 10) {  // Level up every 5 points, max 10 levels
      level_++;
      increaseSnakeSpeed();
    }
    if (snake_.size() >=
        200) {  // Win condition: snake length reaches 200 units
      current_state_ = GAME_OVER_WIN;
      return;
    }
    generateFood();
  }
}

void Game::increaseSnakeSpeed() {
  speed_ = std::max(
      100, speed_ - 40);  // Minimum speed 100ms, decrease by 40ms per level
}

// --- FSM and Game Logic Update Functions ---

void Game::handleUserInput(UserAction_t action, bool hold) {
  (void)hold;

  switch (current_state_) {
    case START_SCREEN:
      if (action == Start) {
        current_state_ = GAME_RUNNING;
      } else if (action == Terminate) {
        current_state_ = TERMINATE_GAME;
      }
      break;
    case GAME_RUNNING:
      if (action == Pause) {
        current_state_ = PAUSED;
      } else if (action == Terminate) {
        current_state_ = TERMINATE_GAME;
      } else if (action == Left) {
        // Only allow turning left/right relative to current direction
        if (snake_direction_.x != 0) {  // Moving horizontally (left/right)
          // Turn left: if moving right (+1,0) -> up (0,-1); if moving left
          // (-1,0) -> down (0,1)
          snake_direction_ = {0, -snake_direction_.x};
        } else {  // Moving vertically (up/down)
          // Turn left: if moving up (0,-1) -> left (-1,0); if moving down (0,1)
          // -> right (1,0)
          snake_direction_ = {snake_direction_.y, 0};
        }
      } else if (action == Right) {
        if (snake_direction_.x != 0) {  // Moving horizontally (left/right)
          // Turn right: if moving right (+1,0) -> down (0,1); if moving left
          // (-1,0) -> up (0,-1)
          snake_direction_ = {0, snake_direction_.x};
        } else {  // Moving vertically (up/down)
          // Turn right: if moving up (0,-1) -> right (1,0); if moving down
          // (0,1) -> left (-1,0)
          snake_direction_ = {-snake_direction_.y, 0};
        }
      } else if (action == Action) {
        // 'Action' button for speeding up snake movement
        moveSnake();
      }
      break;
    case PAUSED:
      if (action == Pause) {
        current_state_ = GAME_RUNNING;
      } else if (action == Terminate) {
        current_state_ = TERMINATE_GAME;
      } else if (action == Start) {
        resetGame();
      }
      break;
    case GAME_OVER_WIN:
    case GAME_OVER_LOSE:
      if (action == Start) {
        resetGame();
      } else if (action == Terminate) {
        current_state_ = TERMINATE_GAME;
      }
      break;
    case TERMINATE_GAME:
      break;
  }
}

void Game::updateGameLogic() {
  // This function is called periodically by the GUI's QTimer
  // It updates the game state based on the current FSM state.
  if (current_state_ == GAME_RUNNING) {
    moveSnake();
  }
}

GameInfo_t Game::getCurrentState() {
  // This function is called by the GUI to get information for rendering.
  // It also triggers the game logic update if needed.
  updateGameLogic();  // Update game logic before providing the state

  GameInfo_t info;

  // Allocate and copy the main field
  info.field = new int*[FIELD_HEIGHT];
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    info.field[i] = new int[FIELD_WIDTH];
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      info.field[i][j] = game_field_[i][j];
    }
  }

  // Allocate and copy the next field (not used for Snake, but must not be
  // nullptr)
  info.next = new int*[4];
  for (int i = 0; i < 4; ++i) {
    info.next[i] = new int[4];
    for (int j = 0; j < 4; ++j) {
      info.next[i][j] = next_field_[i][j];
    }
  }

  info.score = score_;
  info.high_score = high_score_;
  info.level = level_;
  info.speed = speed_;  // Speed in milliseconds, tells GUI how fast to tick
  info.pause = (current_state_ == PAUSED) ? 1 : 0;
  info.current_game_state = current_state_;

  return info;
}

void Game::loadHighScore() {
  std::ifstream file(kHighScoreFilePath);
  if (file.is_open()) {
    file >> high_score_;
    file.close();
  } else {
    high_score_ = 0;
  }
}

void Game::saveHighScore() {
  std::ofstream file(kHighScoreFilePath);
  if (file.is_open()) {
    file << high_score_;
    file.close();
  }
}

}  // namespace s21