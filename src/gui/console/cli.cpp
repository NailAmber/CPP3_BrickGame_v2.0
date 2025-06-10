#include "cli.h"

// --- ncurses Renderer ---

void draw_game(const game::GameInfo_t& game_info) {
  clear();  // Clear the ncurses screen

  // Define offsets for the game field, if you want it centered or padded
  int start_row = 2;
  int start_col = 2;

  // Sidebar info - position it to the right of the game field
  int sidebar_col =
      start_col + game::FIELD_WIDTH * 2 + 3;  // 3 spaces for margin

  // Draw top border
  mvprintw(start_row - 1, start_col - 1, "+");
  for (int i = 0; i < game::FIELD_WIDTH; ++i) {
    mvprintw(start_row - 1, start_col + i * 2, "--");
  }
  mvprintw(start_row - 1, start_col + game::FIELD_WIDTH * 2, "+");

  // Draw game field and sidebar
  for (int y = 0; y < game::FIELD_HEIGHT; ++y) {
    // Left border
    mvprintw(start_row + y, start_col - 1, "|");

    for (int x = 0; x < game::FIELD_WIDTH; ++x) {
      int screen_x = start_col + x * 2;  // Each game "pixel" is 2 chars wide
      int screen_y = start_row + y;
      switch (game_info.field[y][x]) {
        case game::EMPTY:
          mvprintw(screen_y, screen_x, "  ");
          break;
        case game::HEAD:
          mvprintw(screen_y, screen_x, "@@");
          break;
        case game::BODY:
          mvprintw(screen_y, screen_x, "[]");
          break;
        case game::FOOD:
          mvprintw(screen_y, screen_x, "()");
          break;
        default:
          mvprintw(screen_y, screen_x, "??");
          break;
      }
    }
    // Right border
    mvprintw(start_row + y, start_col + game::FIELD_WIDTH * 2, "|");

    if (y == 0)
      mvprintw(start_row + y, sidebar_col, "Score: %d", game_info.score);
    else if (y == 1)
      mvprintw(start_row + y, sidebar_col, "High Score: %d",
               game_info.high_score);
    else if (y == 2)
      mvprintw(start_row + y, sidebar_col, "Level: %d", game_info.level);
    else if (y == 3)
      mvprintw(start_row + y, sidebar_col, "Speed: %dms", game_info.speed);
    else if (y == 5) {
      if (game_info.current_game_state == game::PAUSED)
        mvprintw(start_row + y, sidebar_col, "--- PAUSED ---");
      else if (game_info.current_game_state == game::START_SCREEN)
        mvprintw(start_row + y, sidebar_col, "Press 'S' to Start");
      else if (game_info.current_game_state == game::GAME_OVER_WIN)
        mvprintw(start_row + y, sidebar_col, "YOU WIN!");
      else if (game_info.current_game_state == game::GAME_OVER_LOSE)
        mvprintw(start_row + y, sidebar_col, "GAME OVER!");
    } else if (y == 6) {
      if (game_info.current_game_state == game::PAUSED)
        mvprintw(start_row + y, sidebar_col, "Press 'P' to Resume");
      else if (game_info.current_game_state == game::GAME_OVER_WIN ||
               game_info.current_game_state == game::GAME_OVER_LOSE)
        mvprintw(start_row + y, sidebar_col, "Press 'S' to Restart");
    } else if (y == 9)
      mvprintw(start_row + y, sidebar_col, "Press 'Q' to Quit");
  }

  for (int next_field_y = 0; next_field_y < game::NEXT_FIELD_HEIGHT;
       ++next_field_y) {
    for (int next_field_x = 0; next_field_x < game::NEXT_FIELD_WIDTH;
         ++next_field_x) {
      int screen_x = sidebar_col + 1 +
                     next_field_x * 2;  // Each game "pixel" is 2 chars wide
      int screen_y = start_row + next_field_y + 11;
      switch (game_info.next[next_field_y][next_field_x]) {
        case game::EMPTY:
          mvprintw(screen_y, screen_x, "  ");
          break;
        case game::HEAD:
          mvprintw(screen_y, screen_x, "@@");
          break;
        case game::BODY:
          mvprintw(screen_y, screen_x, "[]");
          break;
        case game::FOOD:
          mvprintw(screen_y, screen_x, "()");
          break;
        default:
          mvprintw(screen_y, screen_x, "??");
          break;
      }
    }
  }

  // Draw bottom border
  int bottom_row = start_row + game::FIELD_HEIGHT;
  mvprintw(bottom_row, start_col - 1, "+");
  for (int i = 0; i < game::FIELD_WIDTH; ++i) {
    mvprintw(bottom_row, start_col + i * 2, "--");
  }
  mvprintw(bottom_row, start_col + game::FIELD_WIDTH * 2, "+");

  refresh();  // Update the physical screen
}

// --- Main Game Loop ---

int main() {
  // Initialize ncurses
  initscr();              // Start ncurses mode
  cbreak();               // Line buffering disabled, Pass on evertyhing
  noecho();               // Don't echo() while we do getch
  keypad(stdscr, TRUE);   // Enable Fx keys, arrow keys, etc.
  nodelay(stdscr, TRUE);  // getch() will be non-blocking
  curs_set(0);            // Make cursor invisible

  game::GameInfo_t game_info;

  bool running = true;

  while (running) {
    // 1. Process Input
    int input_key = getch();  // Read key press (non-blocking)
    flushinp();
    game::UserAction_t action = game::Action;  // Default action

    if (input_key != ERR) {  // ERR means no key was pressed
      switch (input_key) {
        case 'w':
        case 'W':
        case KEY_UP:  // ncurses constant for Up Arrow
          action = game::Up;
          break;
        case 'a':
        case 'A':
        case KEY_LEFT:  // ncurses constant for Left Arrow
          action = game::Left;
          break;
        case 's':
        case 'S':
          action = game::Start;
          break;
        case 'd':
        case 'D':
        case KEY_RIGHT:  // ncurses constant for Right Arrow
          action = game::Right;
          break;
        // Adding explicit Down action if needed, e.g. with KEY_DOWN
        case KEY_DOWN:  // ncurses constant for Down Arrow
                        // (Original code used 'S' for start, not explicitly for
                        // moving down)
          action = game::Down;
          break;
        case 'p':
        case 'P':
          action = game::Pause;
          break;
        case ' ':                 // Space bar
          action = game::Action;  // Assuming this is for "speed up" or
                                  // generic action
          break;
        case 'q':
        case 'Q':
          action = game::Terminate;
          break;
        default:
          // Optional: handle other keys or ignore
          break;
      }
      s21_controller::userInput(action, false);  // Pass action to game model
    }

    // 2. Update Game State & Get Info for Rendering
    game_info = s21_controller::updateCurrentState();

    // 3. Render
    draw_game(game_info);

    // 4. Check for game termination
    if (game_info.current_game_state == game::TERMINATE_GAME) {
      running = false;
    }
    // 5. Control Game Speed
    std::this_thread::sleep_for(std::chrono::milliseconds(game_info.speed));
  }

  // Cleanup ncurses
  endwin();  // Restore terminal settings

  return 0;
}
