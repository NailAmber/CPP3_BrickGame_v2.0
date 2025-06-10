#ifndef GUI_H
#define GUI_H

#include <QApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QPainter>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "../../brick_game/GameController.h"

extern const int GUI_MAIN_BOARD_BLOCK_SIZE;
extern const int GUI_PREVIEW_BLOCK_SIZE;
extern const int GUI_PREVIEW_GRID_DIMENSION;

/**
 * @brief Widget for displaying the main game board.
 */
class GameBoardWidget : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief Constructs the main game board widget.
   * @param parent Parent widget.
   */
  explicit GameBoardWidget(QWidget *parent = nullptr);

  /**
   * @brief Updates the board display with new game info.
   * @param game_info Pointer to the current game info struct.
   */
  void updateBoardDisplay(const s21::GameInfo_t *game_info);

 protected:
  /**
   * @brief Handles the paint event to render the game board.
   * @param event Paint event.
   */
  void paintEvent(QPaintEvent *event) override;

 private:
  const s21::GameInfo_t
      *current_game_data_ptr;  ///< Pointer to current game data.
};

/**
 * @brief Widget for displaying the preview of the next item or piece.
 */
class GamePreviewWidget : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief Constructs the preview widget.
   * @param parent Parent widget.
   */
  explicit GamePreviewWidget(QWidget *parent = nullptr);

  /**
   * @brief Updates the preview display with new game info.
   * @param game_info Pointer to the current game info struct.
   */
  void updatePreviewDisplay(const s21::GameInfo_t *game_info);

 protected:
  /**
   * @brief Handles the paint event to render the preview.
   * @param event Paint event.
   */
  void paintEvent(QPaintEvent *event) override;

 private:
  int **preview_data_ptr;  ///< Pointer to preview data.
};

/**
 * @brief Main window for the game GUI, manages all widgets and game loop.
 */
class GameMainWindow : public QMainWindow {
  Q_OBJECT
 public:
  /**
   * @brief Constructs the main window.
   * @param parent Parent widget.
   */
  explicit GameMainWindow(QWidget *parent = nullptr);

  /**
   * @brief Destructor for cleanup.
   */
  ~GameMainWindow();

 protected:
  /**
   * @brief Handles key press events for user input.
   * @param event Key event.
   */
  void keyPressEvent(QKeyEvent *event) override;

 private slots:
  /**
   * @brief Slot called on each game tick (timer event).
   */
  void onGameTick();

 private:
  GameBoardWidget *mainGameBoardWidget;  ///< Main game board widget.
  GamePreviewWidget *itemPreviewWidget;  ///< Preview widget for next item.
  QLabel *scoreDisplayLabel;             ///< Displays current score.
  QLabel *highScoreDisplayLabel;         ///< Displays high score.
  QLabel *levelDisplayLabel;             ///< Displays current level.
  QLabel *speedDisplayLabel;             ///< Displays current speed.
  QLabel
      *gameStatusDisplayLabel;  ///< Displays game status (paused, over, etc).
  QTimer *gameLoopTimer;        ///< Timer for game loop.
  s21::GameInfo_t current_game_info_struct;  ///< Holds current game info.

  /**
   * @brief Sets up the user interface components.
   */
  void setupUI();

  /**
   * @brief Cleans up any allocated memory for game info data.
   */
  void cleanupAllocatedGameInfoData();

  /**
   * @brief Refreshes the UI display with the latest game info.
   */
  void refreshUIDisplay();

  /**
   * @brief Updates the timer interval based on the current game state.
   */
  void updateTimerBasedOnGameState();
};
#endif  // GUI_H