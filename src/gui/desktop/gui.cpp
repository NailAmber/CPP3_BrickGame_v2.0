#include "gui.h"

const int GUI_MAIN_BOARD_BLOCK_SIZE = 25;
const int GUI_PREVIEW_BLOCK_SIZE = 20;
const int GUI_PREVIEW_GRID_DIMENSION = 4;

// GameBoardWidget Implementation
GameBoardWidget::GameBoardWidget(QWidget *parent)
    : QWidget(parent), current_game_data_ptr(nullptr) {
  setFixedSize(s21::FIELD_WIDTH * GUI_MAIN_BOARD_BLOCK_SIZE + 2,
               s21::FIELD_HEIGHT * GUI_MAIN_BOARD_BLOCK_SIZE + 2);
}

void GameBoardWidget::updateBoardDisplay(const s21::GameInfo_t *game_info) {
  current_game_data_ptr = game_info;
  update();
}

void GameBoardWidget::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::white);
  painter.drawRect(0, 0, width() - 1, height() - 1);
  if (!current_game_data_ptr || !current_game_data_ptr->field) {
    painter.fillRect(rect().adjusted(1, 1, -1, -1), Qt::black);
    painter.setPen(Qt::gray);
    painter.drawText(rect(), Qt::AlignCenter, "No Board Data");
    return;
  }
  for (int r = 0; r < s21::FIELD_HEIGHT; ++r) {
    for (int c = 0; c < s21::FIELD_WIDTH; ++c) {
      QRect blockRect(c * GUI_MAIN_BOARD_BLOCK_SIZE + 1,
                      r * GUI_MAIN_BOARD_BLOCK_SIZE + 1,
                      GUI_MAIN_BOARD_BLOCK_SIZE, GUI_MAIN_BOARD_BLOCK_SIZE);
      QColor blockColor = Qt::black;
      QString blockText = "";
      bool drawFill = true;
      switch (current_game_data_ptr->field[r][c]) {
        case s21::EMPTY:
          drawFill = false;
          painter.setPen(QColor(40, 40, 40));
          painter.drawRect(blockRect);
          break;
        case s21::BODY:
          blockColor = Qt::cyan;
          break;
        case s21::HEAD:
          blockColor = Qt::green;
          break;
        case s21::FOOD:
          blockColor = Qt::red;
          break;
        default:
          blockColor = Qt::darkMagenta;
          blockText = "?";
          break;
      }
      if (drawFill) {
        painter.fillRect(blockRect.adjusted(1, 1, -1, -1), blockColor);
        painter.setPen(blockColor.darker(120));
        painter.drawRect(blockRect.adjusted(1, 1, -1, -1));
        if (!blockText.isEmpty()) {
          painter.setPen(Qt::white);
          painter.drawText(blockRect, Qt::AlignCenter, blockText);
        }
      }
    }
  }
}

// GamePreviewWidget Implementation
GamePreviewWidget::GamePreviewWidget(QWidget *parent)
    : QWidget(parent), preview_data_ptr(nullptr) {
  setFixedSize(GUI_PREVIEW_GRID_DIMENSION * GUI_PREVIEW_BLOCK_SIZE + 2,
               GUI_PREVIEW_GRID_DIMENSION * GUI_PREVIEW_BLOCK_SIZE + 2);
}

void GamePreviewWidget::updatePreviewDisplay(const s21::GameInfo_t *game_info) {
  if (game_info && game_info->next) {
    preview_data_ptr = game_info->next;
  } else {
    preview_data_ptr = nullptr;
  }
  update();
}

void GamePreviewWidget::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::white);
  painter.drawRect(0, 0, width() - 1, height() - 1);
  painter.fillRect(rect().adjusted(1, 1, -1, -1), Qt::black);
  if (!preview_data_ptr) return;
  for (int r = 0; r < GUI_PREVIEW_GRID_DIMENSION; ++r) {
    for (int c = 0; c < GUI_PREVIEW_GRID_DIMENSION; ++c) {
      QRect blockRect(c * GUI_PREVIEW_BLOCK_SIZE + 1,
                      r * GUI_PREVIEW_BLOCK_SIZE + 1, GUI_PREVIEW_BLOCK_SIZE,
                      GUI_PREVIEW_BLOCK_SIZE);
      QColor blockColor = Qt::black;
      bool drawFill = true;
      switch (preview_data_ptr[r][c]) {
        case s21::EMPTY:
          drawFill = false;
          break;
        case s21::BODY:
          blockColor = Qt::yellow;
          break;
        case s21::HEAD:
          blockColor = Qt::darkGreen;
          break;
        case s21::FOOD:
          blockColor = Qt::darkRed;
          break;
        default:
          blockColor = Qt::gray;
          break;
      }
      if (drawFill) {
        painter.fillRect(blockRect.adjusted(1, 1, -1, -1), blockColor);
        painter.setPen(blockColor.darker(120));
        painter.drawRect(blockRect.adjusted(1, 1, -1, -1));
      }
    }
  }
}

// GameMainWindow Implementation
GameMainWindow::GameMainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Qt Generic Game GUI");
  current_game_info_struct.field = nullptr;
  current_game_info_struct.next = nullptr;
  current_game_info_struct.pause = 0;
  current_game_info_struct.current_game_state = s21::START_SCREEN;
  setupUI();
  gameLoopTimer = new QTimer(this);
  connect(gameLoopTimer, &QTimer::timeout, this, &GameMainWindow::onGameTick);
  current_game_info_struct = s21_controller::updateCurrentState();
  refreshUIDisplay();
  updateTimerBasedOnGameState();
  setFocusPolicy(Qt::StrongFocus);
  setFocus();
}

GameMainWindow::~GameMainWindow() { cleanupAllocatedGameInfoData(); }

void GameMainWindow::keyPressEvent(QKeyEvent *event) {
  s21::UserAction_t action_to_send = s21::Action;
  bool relevant_key = true;
  bool pause_key = false;
  switch (event->key()) {
    case Qt::Key_A:
    case Qt::Key_Left:
      action_to_send = s21::Left;
      break;
    case Qt::Key_D:
    case Qt::Key_Right:
      action_to_send = s21::Right;
      break;
    case Qt::Key_S:
      if (current_game_info_struct.current_game_state == s21::START_SCREEN ||
          current_game_info_struct.current_game_state == s21::GAME_OVER_LOSE ||
          current_game_info_struct.current_game_state == s21::GAME_OVER_WIN) {
        action_to_send = s21::Start;
      } else {
        action_to_send = s21::Down;
      }
      break;
    case Qt::Key_Down:
      action_to_send = s21::Down;
      break;
    case Qt::Key_W:
    case Qt::Key_Up:
      action_to_send = s21::Up;
      break;
    case Qt::Key_Space:
      action_to_send = s21::Action;
      break;
    case Qt::Key_P:
      action_to_send = s21::Pause;
      pause_key = true;
      break;
    case Qt::Key_Q:
    case Qt::Key_Escape:
      action_to_send = s21::Terminate;
      break;
    default:
      relevant_key = false;
      QMainWindow::keyPressEvent(event);
      break;
  }
  if (relevant_key) {
    s21_controller::userInput(action_to_send, event->isAutoRepeat());
    if (pause_key || action_to_send == s21::Start ||
        action_to_send == s21::Terminate) {
      // Only update the pause UI instantly, not the whole game state
      GameMainWindow::onGameTick();
    }
    // Do NOT update game state or UI for other keys here!
  }
}

void GameMainWindow::onGameTick() {
  cleanupAllocatedGameInfoData();
  current_game_info_struct = s21_controller::updateCurrentState();
  refreshUIDisplay();
  updateTimerBasedOnGameState();
}

void GameMainWindow::setupUI() {
  QWidget *centralAreaWidget = new QWidget(this);
  QHBoxLayout *mainHorizontalLayout = new QHBoxLayout(centralAreaWidget);
  mainGameBoardWidget = new GameBoardWidget(this);
  QWidget *sidebarAreaWidget = new QWidget(this);
  QVBoxLayout *sidebarVerticalLayout = new QVBoxLayout(sidebarAreaWidget);
  sidebarAreaWidget->setFixedWidth(220);
  scoreDisplayLabel = new QLabel("Score: 0");
  highScoreDisplayLabel = new QLabel("High Score: 0");
  levelDisplayLabel = new QLabel("Level: 1");
  speedDisplayLabel = new QLabel("Speed: ---ms");
  QLabel *previewAreaTitleLabel = new QLabel("<b>PREVIEW:</b>");
  itemPreviewWidget = new GamePreviewWidget(this);
  gameStatusDisplayLabel = new QLabel("Press 'S' to Start");
  gameStatusDisplayLabel->setWordWrap(true);
  gameStatusDisplayLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  gameStatusDisplayLabel->setMinimumHeight(120);
  sidebarVerticalLayout->addWidget(scoreDisplayLabel);
  sidebarVerticalLayout->addWidget(highScoreDisplayLabel);
  sidebarVerticalLayout->addWidget(levelDisplayLabel);
  sidebarVerticalLayout->addWidget(speedDisplayLabel);
  sidebarVerticalLayout->addSpacing(15);
  sidebarVerticalLayout->addWidget(previewAreaTitleLabel);
  sidebarVerticalLayout->addWidget(itemPreviewWidget);
  sidebarVerticalLayout->addSpacing(15);
  sidebarVerticalLayout->addWidget(gameStatusDisplayLabel);
  sidebarVerticalLayout->addStretch();
  mainHorizontalLayout->addWidget(mainGameBoardWidget);
  mainHorizontalLayout->addWidget(sidebarAreaWidget);
  setCentralWidget(centralAreaWidget);
  adjustSize();
}

void GameMainWindow::cleanupAllocatedGameInfoData() {
  if (current_game_info_struct.field) {
    for (int i = 0; i < s21::FIELD_HEIGHT; ++i) {
      if (current_game_info_struct.field[i])
        free(current_game_info_struct.field[i]);
    }
    free(current_game_info_struct.field);
    current_game_info_struct.field = nullptr;
  }
  if (current_game_info_struct.next) {
    for (int i = 0; i < GUI_PREVIEW_GRID_DIMENSION; ++i) {
      if (current_game_info_struct.next[i])
        free(current_game_info_struct.next[i]);
    }
    free(current_game_info_struct.next);
    current_game_info_struct.next = nullptr;
  }
}

void GameMainWindow::refreshUIDisplay() {
  mainGameBoardWidget->updateBoardDisplay(&current_game_info_struct);
  itemPreviewWidget->updatePreviewDisplay(&current_game_info_struct);
  scoreDisplayLabel->setText(
      QString("Score: %1").arg(current_game_info_struct.score));
  highScoreDisplayLabel->setText(
      QString("High Score: %1").arg(current_game_info_struct.high_score));
  levelDisplayLabel->setText(
      QString("Level: %1").arg(current_game_info_struct.level));
  speedDisplayLabel->setText(
      QString("Speed: %1ms").arg(current_game_info_struct.speed));
  QString statusText;
  const s21::GameInfo_t &info = current_game_info_struct;
  if (info.current_game_state == s21::PAUSED) {
    statusText += "--- PAUSED ---\n";
  } else if (info.current_game_state == s21::START_SCREEN) {
    statusText += "Press 'S' to Start\n";
  } else if (info.current_game_state == s21::GAME_OVER_WIN) {
    statusText += "YOU WIN!\n";
  } else if (info.current_game_state == s21::GAME_OVER_LOSE) {
    statusText += "GAME OVER!\n";
  }
  if (info.current_game_state == s21::PAUSED) {
    statusText += "Press 'P' to Resume\n";
  } else if (info.current_game_state == s21::GAME_OVER_WIN ||
             info.current_game_state == s21::GAME_OVER_LOSE) {
    statusText += "Press 'S' to Restart\n";
  }
  statusText += "\nPress 'Q' to Quit";
  gameStatusDisplayLabel->setText(statusText.trimmed());
}

void GameMainWindow::updateTimerBasedOnGameState() {
  if (current_game_info_struct.current_game_state == s21::GAME_RUNNING &&
      !current_game_info_struct.pause) {
    if (current_game_info_struct.speed > 0) {
      if (!gameLoopTimer->isActive() ||
          gameLoopTimer->interval() != current_game_info_struct.speed) {
        gameLoopTimer->start(current_game_info_struct.speed);
      }
    } else {
      gameLoopTimer->stop();
    }
  } else {
    gameLoopTimer->stop();
  }
  if (current_game_info_struct.current_game_state == s21::TERMINATE_GAME) {
    QTimer::singleShot(150, this, &GameMainWindow::close);
  }
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  GameMainWindow mainWindow;
  mainWindow.show();
  return app.exec();
}