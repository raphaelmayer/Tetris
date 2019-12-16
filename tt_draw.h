#ifndef TT_DRAW_H
#define TT_DRAW_H

#include "tt_types.h"

/** Defines the number of main menu items available to be selected. */
#define NUM_MAIN_MENU 4

/** Defines the total height of program window to be rendered in the back. */
#define MAIN_WIN_Y 30
/** Defines the total width of program window to be rendered in the back. */
#define MAIN_WIN_X 80

/** Defines the height of any popup windows such as the help or gaming one. */
#define SUB_WIN_Y 20
/** Defines the width of any popup windows such as the help or gaming one. */
#define SUB_WIN_X 40

/** The character that should be used for free / not occupied pixels on the tetris board. */
#define CHAR_EMPTY '-'
/** The character that should be used for occupied pixels on the tetris board. */
#define CHAR_OCCUPIED 'O'

/**
 * Initializes all needed gaming windows for the program and stores them in the tetris struct.
 * @param tetris tt_tetris struct into which the gaming windows should be stored.
 * @return false if any window has failed to be created.
 */
bool dw_init_windows(tt_tetris *tetris);

/**
 * Draws the game menu together with the cursor on the currently selected item.
 * @param tetris
 * @param menuitem the currently selected item.
 */
void dw_draw_main_menu(tt_tetris *tetris, cursor_main_menu menuitem);

/**
 * Draws the game over window together with the score reached this round.
 * @param tetris
 */
void dw_draw_game_over(tt_tetris *tetris);

/**
 * Draws the game window together with the board and the tetris block currently falling.
 * @param tetris
 */
void dw_draw_game_window(tt_tetris *tetris);

/**
 * Function that acts similar to a popup in any browser.
 * It draws the window w_next over the current window.
 * As soon as any button has been pressed window w_next_next will be drawn.
 * @param tetris
 */
void dw_show_static_window(WINDOW *w_next, WINDOW *w_next_next);

/**
 * Frees all windows from memory and restores the normal terminal settings, previously manipulated
 * by ncurses.
 * @param tetris
 */
void dw_delete_windows(tt_tetris *tetris);

#endif // TT_DRAW_H
