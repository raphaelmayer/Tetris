#ifndef TT_TYPES_H
#define TT_TYPES_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/** Defines the height of the tetris board. */
#define BOARD_Y 20
/** Defines the width of the tetris board. */
#define BOARD_X 11

/** Defines the initial falling speed of a tetris block. */
#define INIT_SPEED 500000

/** Time delay between the frames rendered. This waiting is performed at every wgetch call. */
#define TIME_DELAY 10

/**
 * Enum to store the state of the currently selected main menu item.
 */
typedef enum { NEW_GAME, HIGH_SCORE, HELP_MENU, QUIT } cursor_main_menu;

/**
 * Enum to list all possible block movements.
 */
enum tt_movement { TT_LEFT, TT_RIGHT, TT_DOWN, TT_FALL_DOWN, TT_ROTATE };

/**
 * Packs all needed information about a block into a struct.
 * This includes the current block position [x, y] as well as the squared bounding box of the block.
 * The box shape is stored inside the array.
 */
typedef struct {
	int width, y, x;
	char array[5][5];
} tetris_block;

/**
 * Packs all needed information about the whole program into one big struct
 * In the whole program a pointer of this struct is given to different functions
 * with which different gaming properties can be manipulated.
 *
 * The information stored into this struct are:
 *  - the current tetris board as an array, which stores all free or occupied pixels
 *  - the upcoming falling block
 *  - the currently falling block
 *  - the score of the current game
 *  - the current falling speed of the blocks
 *
 *  - four different windows that can be rendered with ncurses
 */
typedef struct {
	char board[BOARD_Y][BOARD_X];
	tetris_block next_block;
	tetris_block current_block;
	unsigned score;
	unsigned speed;

	WINDOW *w_main;
	WINDOW *w_help;
	WINDOW *w_game;
	WINDOW *w_game_over;
} tt_tetris;

#endif // TT_TYPES_H
