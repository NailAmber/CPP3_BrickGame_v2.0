# snake_gui.pro

QT       += core gui widgets
CONFIG   += c++20 console
TARGET   = snake_gui  # Name of your executable
TEMPLATE = app

# The single C++ source file for the GUI
SOURCES += gui.cpp
HEADERS += gui.h

SOURCES += ../../brick_game/snake/snake.cpp ../../brick_game/GameController.cpp

# Assuming game_controller.h and GameCommon.h are in a directory
INCLUDEPATH += ../../brick_game ../../brick_game/snake # Or wherever your headers are