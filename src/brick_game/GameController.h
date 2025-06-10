#ifndef GAME_CONTROLLER_H_
#define GAME_CONTROLLER_H_

#include "GameCommon.h"

namespace s21_controller {
extern void userInput(s21::UserAction_t action,
                      bool hold);  // Pass action to game model
extern s21::GameInfo_t updateCurrentState();
}  // namespace s21_controller

#endif  // GAME_CONTROLLER_H_