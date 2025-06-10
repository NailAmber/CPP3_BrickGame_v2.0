#include "GameController.h"

namespace s21_controller {
void userInput(s21::UserAction_t action, bool hold) {
  s21::userInput(action, hold);
}
s21::GameInfo_t updateCurrentState() { return s21::updateCurrentState(); }
}  // namespace s21_controller