# tetris_gui.pro

QT       += core gui widgets
CONFIG   += c11 console
TARGET   = tetris_gui  # Name of your executable
TEMPLATE = app

# The single C++ source file for the GUI
SOURCES += gui.cpp
HEADERS += gui.h

SOURCES += ../../brick_game/tetris/tetris.c ../../brick_game/GameController.cpp

# Assuming game_controller.h and GameCommon.h are in a directory
INCLUDEPATH += ../../brick_game ../../brick_game/tetris

# Specific C flags:
c_sources.flags = -std=c11 -Werror # Or other C flags
QMAKE_CFLAGS += -Wno-unused-parameter -Wno-implicit-fallthrough