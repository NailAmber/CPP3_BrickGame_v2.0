# Makefile for BrickGame v2.0 (Snake and Tetris)

# Compiler
CXX = g++
CC = gcc

# C++ Standard
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic
CCFLAGS = -std=c11 -Werror

# Gtest and coverage flags
GTEST_LIBS = -lgtest -pthread
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage --coverage


# Cppcheck flags for full static check
CPPCHECKFLAGS = --enable=all --std=c++20 -x c++ --suppress=missingIncludeSystem \
				--suppress=unusedStructMember --suppress=unknownMacro --suppress=checkersReport \
				$(SNAKE_DIR)/* $(CONSOLE_GUI_DIR)/* \
				$(DESKTOP_GUI_DIR)/*.h $(DESKTOP_GUI_DIR)/*.cpp \
				$(BRICK_GAME_DIR)/*.cpp $(BRICK_GAME_DIR)/*.h $(TEST_SRC)

# clang-format flags for full style format check
CLANGFORMATFLAGS = $(SNAKE_DIR)/* $(CONSOLE_GUI_DIR)/* $(DESKTOP_GUI_DIR)/*.h $(TEST_SRC) \
					$(DESKTOP_GUI_DIR)/*.cpp $(BRICK_GAME_DIR)/*.cpp $(BRICK_GAME_DIR)/*.h \
					--style=Google

# Libraries for console GUI
LDFLAGS = -lncursesw

# Directories
SRC_DIR = ./
BRICK_GAME_DIR = $(SRC_DIR)/brick_game
SNAKE_DIR = $(BRICK_GAME_DIR)/snake
TETRIS_DIR = $(BRICK_GAME_DIR)/tetris
GUI_DIR = $(SRC_DIR)/gui
CONSOLE_GUI_DIR = $(GUI_DIR)/console
DESKTOP_GUI_DIR = $(GUI_DIR)/desktop
TEST_DIR = tests

# Output directories
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
LIB_DIR = $(BUILD_DIR)/lib
OBJ_DIR = $(BUILD_DIR)/obj
DIST_DIR = dist
DOCS_DIR = docs
LATEX_DIR = latex
DOXYGEN_HTML_DIR = $(DOCS_DIR)/html

# Executables
SNAKE_CONSOLE_APP = $(BIN_DIR)/snake_cli
SNAKE_DESKTOP_APP = $(BIN_DIR)/snake_gui
TETRIS_CONSOLE_APP = $(BIN_DIR)/tetris_cli
TETRIS_DESKTOP_APP = $(BIN_DIR)/tetris_gui
TEST_APP = $(TEST_DIR)/snake_test

# Library (static library for game logic)
SNAKE_LIB = $(LIB_DIR)/libsnake.a
TETRIS_LIB = $(LIB_DIR)/libtetris.a

# Source files
CONTROLLER_MAIN_SRC = $(BRICK_GAME_DIR)/GameController.cpp
SNAKE_SRC = $(SNAKE_DIR)/snake.cpp
TETRIS_SRC = $(TETRIS_DIR)/tetris.c
CONSOLE_MAIN_SRC = $(CONSOLE_GUI_DIR)/cli.cpp
DOXYFILE_SRC = Doxyfile

# Doxygen's html out
HTML_OUT = $(DOCS_DIR)/$(DOC_BASE_NAME).html

# Tarball out
DIST_TAR = brickgame_dist.tar.gz

# Object files
CONTROLLER_OBJS = $(patsubst $(BRICK_GAME_DIR)/%.cpp,$(OBJ_DIR)/controller_%.o,$(CONTROLLER_SRCS))
SNAKE_OBJS = $(patsubst $(SNAKE_DIR)/%.cpp,$(OBJ_DIR)/model_snake_%.o,$(SNAKE_SRC))
TETRIS_OBJS = $(patsubst $(TETRIS_DIR)/%.c,$(OBJ_DIR)/model_tetris_%.o,$(TETRIS_SRC))

# Separate object files for main.cpp for each game
CONTROLLER_SNAKE_OBJ = $(OBJ_DIR)/controller_snake.o
CONTROLLER_TETRIS_OBJ = $(OBJ_DIR)/controller_tetris.o

# Test source and objects
TEST_SRC = $(TEST_DIR)/snake_test.cpp
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/test_%.o,$(TEST_SRC))


# --- Targets ---

.PHONY: all snake_gui tetris_gui snake_cli tetris_cli \
 		clean install uninstall test dist dvi \
 		run_snake_cli run_tetris_cli run_snake_gui run_tetris_gui \
		open_html

all: snake_gui tetris_gui snake_cli tetris_cli

snake_gui: clean $(BIN_DIR) $(LIB_DIR) $(OBJ_DIR)
	@cd $(DESKTOP_GUI_DIR) && qmake snake_gui.pro && make
	@mv $(DESKTOP_GUI_DIR)/snake_gui $(BIN_DIR)
	@mv $(DESKTOP_GUI_DIR)/*.o $(OBJ_DIR)

tetris_gui: clean $(BIN_DIR) $(LIB_DIR) $(OBJ_DIR)
	@cd $(DESKTOP_GUI_DIR) && qmake tetris_gui.pro && make
	@mv $(DESKTOP_GUI_DIR)/tetris_gui $(BIN_DIR)
	@mv $(DESKTOP_GUI_DIR)/*.o $(OBJ_DIR)

snake_cli: clean $(BIN_DIR) $(LIB_DIR) $(OBJ_DIR) $(SNAKE_CONSOLE_APP)

tetris_cli: clean $(BIN_DIR) $(LIB_DIR) $(OBJ_DIR) $(TETRIS_CONSOLE_APP)

$(BIN_DIR):
	@mkdir -p $@

$(LIB_DIR):
	@mkdir -p $@

$(OBJ_DIR):
	@mkdir -p $@

# Rule to build the Snake game logic static library
$(SNAKE_LIB): $(SNAKE_OBJS) $(CONTROLLER_SNAKE_OBJ)
	ar rcs $@ $^

# Rule to build the Tetris game logic static library
$(TETRIS_LIB): $(TETRIS_OBJS) $(CONTROLLER_SNAKE_OBJ)
	ar rcs $@ $^

# Rule to build the Snake console application
$(SNAKE_CONSOLE_APP): $(CONTROLLER_SNAKE_OBJ) $(SNAKE_LIB) $(CONSOLE_MAIN_SRC)
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) $^ -o $@ $(LDFLAGS)

# Rule to build the Tetris console application
$(TETRIS_CONSOLE_APP): $(CONTROLLER_TETRIS_OBJ) $(TETRIS_LIB) $(CONSOLE_MAIN_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Rule to compile Snake game logic object files
$(OBJ_DIR)/model_snake_%.o: $(SNAKE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c $< -o $@

# Rule to compile Tetris game logic object files
$(OBJ_DIR)/model_tetris_%.o: $(TETRIS_DIR)/%.c
	$(CC) $(CCFLAGS) -c $< -o $@

# Rule to compile test source files into object files
$(OBJ_DIR)/test_%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -I$(TEST_DIR) -I$(BRICK_GAME_DIR) -I$(SNAKE_DIR) -c $< -o $@

# Rule to compile GameController.cpp specifically for the Snake game
$(CONTROLLER_SNAKE_OBJ): $(CONTROLLER_MAIN_SRC)
	$(CXX) $(CXXFLAGS) -I$(SNAKE_DIR) -I$(BRICK_GAME_DIR) -c $< -o $@

# Rule to compile GameController.cpp specifically for the Tetris game
$(CONTROLLER_TETRIS_OBJ): $(CONTROLLER_MAIN_SRC)
	$(CXX) $(CXXFLAGS) -I$(TETRIS_DIR) -I$(BRICK_GAME_DIR) -c $< -o $@

# Test target
test: clean $(OBJ_DIR) $(TEST_APP) coverage

# Rule to link object files into the final test executable
$(TEST_APP): $(SNAKE_OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) $^ -o $@ $(GTEST_LIBS)

# Coverage target to run tests and generate report
coverage:
	@echo "--- Running tests to generate coverage data ---"
	@./$(TEST_APP)
	@echo "--- Generating coverage report ---"
	@gcovr -r $(SRC_DIR) --html --html-details $(TEST_DIR)/coverage.html --gcov-executable gcov-11

clean:
	@rm -rf $(BUILD_DIR) $(DIST_DIR)
	@rm -f $(TEST_APP) $(SNAKE_CONSOLE_APP) $(TETRIS_CONSOLE_APP) $(SNAKE_LIB) $(TETRIS_LIB)
	@rm -f high_score.txt
	@rm -f $(DESKTOP_GUI_DIR)/Makefile $(DESKTOP_GUI_DIR)/.qmake.stash $(DESKTOP_GUI_DIR)/moc*
	@rm -rf $(DOCS_DIR)
	@rm -f $(TEST_DIR)/*.gc* $(TEST_APP) $(TEST_DIR)/coverage.*

install: all
	@echo "Installing BrickGame applications to /usr/local/bin"
	@mkdir -p /usr/local/bin
	@cp $(BIN_DIR)/* /usr/local/bin/

uninstall:
	@echo "Uninstalling BrickGame applications from /usr/local/bin"
	@rm -f /usr/local/bin/$(notdir $(SNAKE_CONSOLE_APP))
	@rm -f /usr/local/bin/$(notdir $(TETRIS_CONSOLE_APP))
	@rm -f /usr/local/bin/$(notdir $(SNAKE_DESKTOP_APP))
	@rm -f /usr/local/bin/$(notdir $(TETRIS_DESKTOP_APP))

dvi: clean $(HTML_OUT)

$(HTML_OUT): $(DOXYFILE_SRC) 
	@doxygen $<

dist: clean
	@echo "Creating distribution package..."
	@mkdir -p $(DIST_DIR)
	@cp -r $(BRICK_GAME_DIR) $(GUI_DIR) $(lastword $(MAKEFILE_LIST)) $(DIST_DIR)
	@cd $(DIST_DIR) && tar -czf ../$(DIST_TAR) *
	@rm -rf $(DIST_DIR)/*
	@mv $(DIST_TAR) $(DIST_DIR)
	@echo "Distribution package created at $(DIST_DIR)/$(DIST_TAR)"

format:
	@clang-format -n $(CLANGFORMATFLAGS)

formati:
	@clang-format -i $(CLANGFORMATFLAGS)

cppcheck:
	@cppcheck $(CPPCHECKFLAGS)

valgrind:
	@valgrind --tool=memcheck --leak-check=yes $(TEST_APP)

run_snake_cli: $(SNAKE_CONSOLE_APP)
	@./$<

run_tetris_cli: $(TETRIS_CONSOLE_APP)
	@./$<

run_snake_gui: $(SNAKE_DESKTOP_APP)
	@./$<

run_tetris_gui: $(TETRIS_DESKTOP_APP)
	@./$<

open_html: $(DOXYGEN_HTML_DIR)/index.html
	@echo "Opening Doxygen HTML documentation..."
	@xdg-open $(DOXYGEN_HTML_DIR)/index.html || open $(DOXYGEN_HTML_DIR)/index.html || start $(DOXYGEN_HTML_DIR)/index.html