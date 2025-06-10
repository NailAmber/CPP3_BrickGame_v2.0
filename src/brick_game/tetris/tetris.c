#include "tetris.h"
#include <stdlib.h> // For malloc, free, rand, srand
#include <string.h> // For memset, memcpy
#include <time.h>   // For srand seeding

// --- Game Constants and Definitions ---

// Tetromino shapes (7 types, 4 rotations each)
// Each piece is defined in a 4x4 grid. 1 means block, 0 means empty.
// Order: I, J, L, O, S, T, Z
const TetrominoShape tetrominoes[NUM_TETROMINO_TYPES][NUM_TETROMINO_ROTATIONS] = {
    // I piece
    {{{{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}}, // Rotation 0 (Horizontal)
     {{{0,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,0,0}}}, // Rotation 1 (Vertical)
     {{{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}}, // Rotation 2
     {{{0,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,0,0}}}},// Rotation 3
    // J piece
    {{{{1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}}, // Rotation 0
     {{{0,1,1,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}}}, // Rotation 1
     {{{0,0,0,0}, {1,1,1,0}, {0,0,1,0}, {0,0,0,0}}}, // Rotation 2
     {{{0,1,0,0}, {0,1,0,0}, {1,1,0,0}, {0,0,0,0}}}},// Rotation 3
    // L piece
    {{{{0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}}, // Rotation 0
     {{{0,1,0,0}, {0,1,0,0}, {0,1,1,0}, {0,0,0,0}}}, // Rotation 1
     {{{0,0,0,0}, {1,1,1,0}, {1,0,0,0}, {0,0,0,0}}}, // Rotation 2
     {{{1,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}}}},// Rotation 3
    // O piece (same for all rotations)
    {{{{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}}, // Rotation 0
     {{{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}}, // Rotation 1
     {{{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}}, // Rotation 2
     {{{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}}},// Rotation 3
    // S piece
    {{{{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}}}, // Rotation 0
     {{{0,1,0,0}, {0,1,1,0}, {0,0,1,0}, {0,0,0,0}}}, // Rotation 1
     {{{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}}}, // Rotation 2
     {{{0,1,0,0}, {0,1,1,0}, {0,0,1,0}, {0,0,0,0}}}},// Rotation 3
    // T piece
    {{{{0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}}, // Rotation 0
     {{{0,1,0,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}}}, // Rotation 1
     {{{0,0,0,0}, {1,1,1,0}, {0,1,0,0}, {0,0,0,0}}}, // Rotation 2
     {{{0,1,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0}}}},// Rotation 3
    // Z piece
    {{{{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}}, // Rotation 0
     {{{0,0,1,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}}}, // Rotation 1
     {{{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}}, // Rotation 2
     {{{0,0,1,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}}}},// Rotation 3
};

// --- Static Global Game State Variables ---
static int game_board[TETRIS_BOARD_HEIGHT][TETRIS_BOARD_WIDTH];
static CurrentPieceState current_piece;
static int next_piece_type;
static int score;
static int high_score;
static int level;
static int game_speed_ms; // Lower is faster
static bool paused;
static TetrisFSMState_t current_fsm_state;
static GameState overall_game_state; // For GameInfo_t

static unsigned long long game_timer_ticks; // Simple timer for piece falling speed
static const int INITIAL_SPEED_MS = 500;
static const int MAX_LEVEL = 10;
static const int POINTS_PER_LEVEL_UP = 600;
static int lines_cleared_for_level_up = 0;


// --- Forward Declarations for Static Helper Functions ---
static void reset_game_state();
static void spawn_new_piece();
static bool is_valid_position(int piece_x, int piece_y, int type, int rotation);
static void lock_current_piece();
static int clear_completed_lines();
static void update_score_and_level(int lines_cleared_count);
static void load_high_score_from_file();
static void save_high_score_to_file();
static void calculate_speed_from_level();
static int **allocate_game_info_field();
static int **allocate_game_info_next_piece_area();
static void copy_board_to_game_info_field(int **dest_field);
static void copy_next_piece_to_game_info_next(int **dest_next);

// --- Initialization ---
void initialize_tetris_game() {
    srand((unsigned int)time(NULL));
    load_high_score_from_file();
    reset_game_state();
    next_piece_type = rand() % NUM_TETROMINO_TYPES;
    current_fsm_state = TETRIS_STATE_START_SCREEN;
    overall_game_state = START_SCREEN; // From GameCommon.h
}

static void reset_game_state() {
    for (int r = 0; r < TETRIS_BOARD_HEIGHT; ++r) {
        for (int c = 0; c < TETRIS_BOARD_WIDTH; ++c) {
            game_board[r][c] = EMPTY; // From GameCommon.h CellState
        }
    }
    current_piece.active = false;
    score = 0;
    level = 1;
    lines_cleared_for_level_up = 0;
    calculate_speed_from_level();
    paused = false;
    game_timer_ticks = 0;

    // Select first piece and next piece
    // current_piece.type = rand() % NUM_TETROMINO_TYPES;
    // current_piece.rotation = 0;
    
    // Note: Actual spawning happens in TETRIS_STATE_SPAWN
}


// --- High Score Handling ---
static void load_high_score_from_file() {
    FILE *fp = fopen(HIGH_SCORE_FILENAME, "r");
    if (fp) {
        if (fscanf(fp, "%d", &high_score) != 1) {
            high_score = 0;
        }
        fclose(fp);
    } else {
        high_score = 0;
    }
}

static void save_high_score_to_file() {
    FILE *fp = fopen(HIGH_SCORE_FILENAME, "w");
    if (fp) {
        fprintf(fp, "%d", high_score);
        fclose(fp);
    }
}

// --- Game Mechanics Helpers ---
static void calculate_speed_from_level() {
    if (level > MAX_LEVEL) level = MAX_LEVEL;
    if (level < 1) level = 1;
    // Example speed calculation: Starts at INITIAL_SPEED_MS, decreases by 40ms per level
    game_speed_ms = INITIAL_SPEED_MS - (level - 1) * 40;
    if (game_speed_ms < 50) game_speed_ms = 50; // Minimum speed
}

static void spawn_new_piece() {
    current_piece.type = next_piece_type;
    next_piece_type = rand() % NUM_TETROMINO_TYPES;

    current_piece.rotation = 0;
    current_piece.x = TETRIS_BOARD_WIDTH / 2 - TETROMINO_GRID_SIZE / 2; // Centered
    current_piece.y = 0; // Start at the top

    current_piece.active = true;

    if (!is_valid_position(current_piece.x, current_piece.y, current_piece.type, current_piece.rotation)) {
        current_fsm_state = TETRIS_STATE_GAME_OVER;
        overall_game_state = GAME_OVER_LOSE;
        current_piece.active = false;
        if (score > high_score) {
            high_score = score;
            save_high_score_to_file();
        }
    } else {
        current_fsm_state = TETRIS_STATE_MOVING;
        overall_game_state = GAME_RUNNING;
    }
    game_timer_ticks = 0; // Reset fall timer
}

static bool is_valid_position(int piece_x, int piece_y, int type, int rotation) {
    const TetrominoShape* s = &tetrominoes[type][rotation];
    for (int r_offset = 0; r_offset < TETROMINO_GRID_SIZE; ++r_offset) {
        for (int c_offset = 0; c_offset < TETROMINO_GRID_SIZE; ++c_offset) {
            if (s->shape[r_offset][c_offset] == 1) { // If part of the piece
                int board_r = piece_y + r_offset;
                int board_c = piece_x + c_offset;

                // Check boundaries
                if (board_c < 0 || board_c >= TETRIS_BOARD_WIDTH || board_r < 0 || board_r >= TETRIS_BOARD_HEIGHT) {
                    return false; // Out of bounds
                }
                // Check collision with existing blocks on the board
                if (game_board[board_r][board_c] != EMPTY) {
                    return false; // Collision with another block
                }
            }
        }
    }
    return true;
}

static void lock_current_piece() {
    if (!current_piece.active) return;

    const TetrominoShape* s = &tetrominoes[current_piece.type][current_piece.rotation];
    for (int r_offset = 0; r_offset < TETROMINO_GRID_SIZE; ++r_offset) {
        for (int c_offset = 0; c_offset < TETROMINO_GRID_SIZE; ++c_offset) {
            if (s->shape[r_offset][c_offset] == 1) {
                int board_r = current_piece.y + r_offset;
                int board_c = current_piece.x + c_offset;
                // Ensure it's within bounds before locking, though is_valid_position should handle this
                if (board_r >= 0 && board_r < TETRIS_BOARD_HEIGHT && board_c >= 0 && board_c < TETRIS_BOARD_WIDTH) {
                     game_board[board_r][board_c] = BODY; // Use BODY from CellState
                }
            }
        }
    }
    current_piece.active = false;
    current_fsm_state = TETRIS_STATE_LINE_CLEAR;
}

static int clear_completed_lines() {
    int lines_cleared_count = 0;
    for (int r = TETRIS_BOARD_HEIGHT - 1; r >= 0; --r) {
        bool line_complete = true;
        for (int c = 0; c < TETRIS_BOARD_WIDTH; ++c) {
            if (game_board[r][c] == EMPTY) {
                line_complete = false;
                break;
            }
        }

        if (line_complete) {
            lines_cleared_count++;
            // Move all lines above this one down
            for (int move_r = r; move_r > 0; --move_r) {
                for (int c = 0; c < TETRIS_BOARD_WIDTH; ++c) {
                    game_board[move_r][c] = game_board[move_r - 1][c];
                }
            }
            // Clear the top line
            for (int c = 0; c < TETRIS_BOARD_WIDTH; ++c) {
                game_board[0][c] = EMPTY;
            }
            r++; // Re-check the current row index as it now contains the row from above
        }
    }
    return lines_cleared_count;
}

static void update_score_and_level(int lines_cleared_count) {
    if (lines_cleared_count > 0) {
        switch (lines_cleared_count) {
            case 1: score += 100; break;
            case 2: score += 300; break;
            case 3: score += 700; break;
            case 4: score += 1500; break; // Tetris!
            default: score += 1500 + (lines_cleared_count - 4) * 800; // Bonus for more than 4
        }
        if (score > high_score) {
            high_score = score; // Update high score in real-time, save on game over
        }

        lines_cleared_for_level_up += lines_cleared_count; // Using lines cleared, not points for level up
                                                           // README: "Each time a player gains 600 points, the level increases by 1"
                                                           // Let's adjust to use points.
        // This logic should be based on score threshold as per README.
        // Example: if previous_score / 600 < current_score / 600, then level up.
        // More simply:
        int old_level_threshold = (score - (lines_cleared_count == 1 ? 100 : (lines_cleared_count == 2 ? 300 : (lines_cleared_count == 3 ? 700 : (lines_cleared_count == 4 ? 1500 : 0))))) / POINTS_PER_LEVEL_UP;
        int new_level_threshold = score / POINTS_PER_LEVEL_UP;

        if (new_level_threshold > old_level_threshold && level < MAX_LEVEL) {
             level = (new_level_threshold) + 1; // Level is 1-based
             if(level > MAX_LEVEL) level = MAX_LEVEL;
             calculate_speed_from_level();
        }
    }
}


// --- Memory Allocation for GameInfo_t ---
static int **allocate_game_info_field() {
    int **field = (int **)malloc(TETRIS_BOARD_HEIGHT * sizeof(int *));
    if (!field) return NULL;
    for (int i = 0; i < TETRIS_BOARD_HEIGHT; ++i) {
        field[i] = (int *)malloc(TETRIS_BOARD_WIDTH * sizeof(int));
        if (!field[i]) {
            // Cleanup previously allocated rows
            for (int k = 0; k < i; ++k) free(field[k]);
            free(field);
            return NULL;
        }
    }
    return field;
}

static int **allocate_game_info_next_piece_area() {
    int **next_area = (int **)malloc(TETROMINO_GRID_SIZE * sizeof(int *));
     if (!next_area) return NULL;
    for (int i = 0; i < TETROMINO_GRID_SIZE; ++i) {
        next_area[i] = (int *)malloc(TETROMINO_GRID_SIZE * sizeof(int));
        if (!next_area[i]) {
            for (int k = 0; k < i; ++k) free(next_area[k]);
            free(next_area);
            return NULL;
        }
    }
    return next_area;
}

static void copy_board_to_game_info_field(int **dest_field) {
    if (!dest_field) return;
    for (int r = 0; r < TETRIS_BOARD_HEIGHT; ++r) {
        for (int c = 0; c < TETRIS_BOARD_WIDTH; ++c) {
            dest_field[r][c] = game_board[r][c];
            // If a piece is active, draw it onto this temporary field
            if (current_piece.active) {
                const TetrominoShape* piece_shape = &tetrominoes[current_piece.type][current_piece.rotation];
                for (int pr = 0; pr < TETROMINO_GRID_SIZE; ++pr) {
                    for (int pc = 0; pc < TETROMINO_GRID_SIZE; ++pc) {
                        if (piece_shape->shape[pr][pc] == 1) {
                            int board_r = current_piece.y + pr;
                            int board_c = current_piece.x + pc;
                            if (board_r == r && board_c == c) {
                                dest_field[r][c] = BODY; // Draw active piece as BODY
                            }
                        }
                    }
                }
            }
        }
    }
}

static void copy_next_piece_to_game_info_next(int **dest_next) {
    if (!dest_next) return;
    const TetrominoShape* s = &tetrominoes[next_piece_type][0]; // Show default rotation
    for (int r = 0; r < TETROMINO_GRID_SIZE; ++r) {
        if (!dest_next[r]) continue; // Prevent possible null pointer dereference
        for (int c = 0; c < TETROMINO_GRID_SIZE; ++c) {
            if (s->shape[r][c] == 1) {
                dest_next[r][c] = BODY;
            } else {
                dest_next[r][c] = EMPTY;
            }
        }
    }
}

// --- API Functions ---

// This is a very basic static instance, not ideal for multiple game instances
// but typical for a simple C library for one game.
static bool is_initialized = false;

void userInput(UserAction_t action, bool hold) {
    if (!is_initialized) { // Lazy initialization
        initialize_tetris_game();
        is_initialized = true;
    }

    if (action == Terminate) {
        current_fsm_state = TETRIS_STATE_GAME_OVER; // Or a specific terminate state
        overall_game_state = TERMINATE_GAME;
        if (score > high_score) { // Save score on terminate too
             save_high_score_to_file();
        }
        return;
    }

    if (action == Start) {
        if (current_fsm_state == TETRIS_STATE_START_SCREEN || current_fsm_state == TETRIS_STATE_GAME_OVER || overall_game_state == PAUSED) {
            reset_game_state(); // Resets board, score, level
            // next_piece_type = rand() % NUM_TETROMINO_TYPES;
            load_high_score_from_file(); // Ensure high score is fresh for new game
            current_fsm_state = TETRIS_STATE_SPAWN;
            overall_game_state = GAME_RUNNING;
        }
        return; // Start action consumes the input here
    }
    
    // Actions below are only valid if game is running and not on start/gameover screen
    if (current_fsm_state == TETRIS_STATE_START_SCREEN || current_fsm_state == TETRIS_STATE_GAME_OVER) {
        return;
    }

    if (action == Pause) {
        paused = !paused;
        if (paused) {
            overall_game_state = PAUSED;
        } else {
            overall_game_state = GAME_RUNNING;
            game_timer_ticks = 0; // Reset timer on unpause to avoid instant drop
        }
        return;
    }

    if (paused) return; // No game actions if paused

    if (current_piece.active && current_fsm_state == TETRIS_STATE_MOVING) {
        int new_x = current_piece.x;
        int new_y = current_piece.y;
        int new_rotation = current_piece.rotation;

        switch (action) {
            case Left:
                new_x--;
                break;
            case Right:
                new_x++;
                break;
            case Down: // Soft drop: move one step down
                new_y++;
                game_timer_ticks = 0; // Reset auto-fall timer, making it feel faster
                break;
            case Action: // Rotate
                new_rotation = (current_piece.rotation + 1) % NUM_TETROMINO_ROTATIONS;
                break;
            default:
                break; // Other actions like Up are not handled here
        }

        if (is_valid_position(new_x, new_y, current_piece.type, new_rotation)) {
            current_piece.x = new_x;
            current_piece.y = new_y;
            current_piece.rotation = new_rotation;
        } else if (action == Down) {
            // If Down action made it invalid, it means it hit something, so lock it
            current_fsm_state = TETRIS_STATE_LOCKING;
        }
    }
}

GameInfo_t updateCurrentState() {
    if (!is_initialized) {
        initialize_tetris_game();
        is_initialized = true;
    }
    
    GameInfo_t info;
    info.field = NULL; // Initialize to NULL
    info.next = NULL;  // Initialize to NULL

    if (!paused && overall_game_state != TERMINATE_GAME && overall_game_state != START_SCREEN && overall_game_state != GAME_OVER_LOSE) {
        game_timer_ticks++;

        // --- FSM Logic ---
        switch (current_fsm_state) {
            case TETRIS_STATE_START_SCREEN:
                // Waiting for Start action via userInput
                overall_game_state = START_SCREEN;
                break;

            case TETRIS_STATE_SPAWN:
                spawn_new_piece(); // This can change state to MOVING or GAME_OVER
                // If still SPAWN (should not happen if spawn_new_piece is correct) or changed to GAMEOVER
                if (current_fsm_state == TETRIS_STATE_GAME_OVER) {
                     overall_game_state = GAME_OVER_LOSE;
                } else {
                     current_fsm_state = TETRIS_STATE_MOVING; // Expected transition
                     overall_game_state = GAME_RUNNING;
                }
                game_timer_ticks = 0;
                break;

            case TETRIS_STATE_MOVING:
                overall_game_state = GAME_RUNNING;
                // Automatic downward movement (gravity)
                // Check ticks against a threshold derived from game_speed_ms
                // Assuming 60 "ticks" per second if updateCurrentState is called frequently enough
                // Or, more simply, if game_speed_ms is the interval:
                // This needs a proper timing mechanism. The CLI sleeps for game_speed_ms.
                // So, each call to updateCurrentState can be considered one "major tick".
                // Let's use a simpler model where game_speed_ms determines how many
                // updateCurrentState calls happen before an auto-drop.
                // If game_speed_ms is, say, 500ms, and updateCurrentState is called every 16ms (60FPS):
                // 500 / 16 ~= 31 ticks.
                // For now, let's assume the CLI loop's sleep(game_speed_ms) implies that
                // each call to updateCurrentState is one "game step" for falling.
                // This is simpler and matches the structure of many such games.
                // So, every time updateCurrentState is called and we are in MOVING state, try to move down.

                if (current_piece.active) {
                    if (is_valid_position(current_piece.x, current_piece.y + 1, current_piece.type, current_piece.rotation)) {
                        current_piece.y++;
                    } else {
                        current_fsm_state = TETRIS_STATE_LOCKING;
                    }
                } else { // Should not happen if logic is correct
                    current_fsm_state = TETRIS_STATE_SPAWN;
                }
                break;

            case TETRIS_STATE_LOCKING:
                lock_current_piece(); // Changes state to LINE_CLEAR
                // No break, fall through to LINE_CLEAR typically
                // fallthrough intended

            case TETRIS_STATE_LINE_CLEAR:
                {
                    int lines = clear_completed_lines();
                    if (lines > 0) {
                        update_score_and_level(lines);
                    }
                    current_fsm_state = TETRIS_STATE_SPAWN; // Get ready for next piece
                }
                break;
            
            case TETRIS_STATE_GAME_OVER:
                overall_game_state = GAME_OVER_LOSE;
                // Persist high score if it changed.
                if (score > high_score) { // This might be redundant if saved on state change
                    high_score = score;
                    save_high_score_to_file();
                }
                break;
        }

    
    } else if (paused && overall_game_state != TERMINATE_GAME) {
        overall_game_state = PAUSED;
    } else if (overall_game_state == START_SCREEN) {
        // Do nothing, wait for start
    } else if (overall_game_state == GAME_OVER_LOSE) {
        // Do nothing, wait for start
    }


    // --- Populate GameInfo_t ---
    info.field = allocate_game_info_field();
    info.next = allocate_game_info_next_piece_area();

    if (info.field) {
        copy_board_to_game_info_field(info.field);
    }
    if (info.next) {
        copy_next_piece_to_game_info_next(info.next);
    }

    info.score = score;
    info.high_score = high_score;
    info.level = level;
    info.speed = game_speed_ms;
    info.pause = paused ? 1 : 0;
    info.current_game_state = overall_game_state;
    
    return info;
}