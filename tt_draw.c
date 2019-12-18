#include "tt_draw.h"

/**
 * A wrapper function for creating a new window with ncurses and
 * printing an error message if it fails.
 * @param pos_y
 * @param pos_x
 * @param size_y
 * @param size_x
 * @return a new window that starts at position [pos_x, pos_y] on the top left
 * corner.
 */
static WINDOW *init_window(int pos_y, int pos_x, int size_y, int size_x) {
	WINDOW *window = newwin(pos_y, pos_x, size_y, size_x);
	if (!window) {
		endwin();
		fprintf(stderr, "Couldn't initialize curses window!\n");
		return NULL;
	}
	return window;
}

/**
 * Function for building a help menu with ncurses.
 * The returned windows has to be freed at the end of its life cycle
 * (dw_delete_windows).
 * @param pos_y
 * @param pos_x
 * @param size_y
 * @param size_x
 * @return a pointer to the initialized help menu.
 */
static WINDOW *init_help_window(int pos_y, int pos_x, int size_y, int size_x) {
	WINDOW *help = init_window(pos_y, pos_x, size_y, size_x);
	if (!help) {
		return NULL;
	}
	char controls[9][2][16] = {
		{ "h", "Help" },
		{ "q", "Quit" },
		{ "+", "Increase speed" },
		{ "-", "Decrease speed" },
		{ "L-arrow", "Move left" },
		{ "R-arrow", "Move right" },
		{ "D-arrow", "Move down" },
		{ "U-arrow", "Rotate" },
		{ "Space", "Fall down" },
	};

	box(help, 0, 0);
	for (int i = 0; i < 9; ++i) {
		mvwprintw(help, SUB_WIN_Y / 6 + i, SUB_WIN_X / 6, "%7s -- %s", controls[i][0],
		          controls[i][1]);
	}
	mvwprintw(help, 0, SUB_WIN_X / 2 - 7, "[ Help menu ]");
	mvwaddstr(help, 5 * SUB_WIN_Y / 6, SUB_WIN_X / 6, "Press ENTER to go back!");
	return help;
}

/**
 * Initializes all needed gaming windows for the program and stores them in the
 * tetris struct.
 * @param tetris tt_tetris struct into which the gaming windows should be
 * stored.
 * @return false if any window has failed to be created.
 */
bool dw_init_windows(tt_tetris *tetris) {
	tetris->w_help = init_help_window(SUB_WIN_Y, SUB_WIN_X, (MAIN_WIN_Y - SUB_WIN_Y) / 2,
	                                  (MAIN_WIN_X - SUB_WIN_X) / 2);
	tetris->w_game_over = init_window(SUB_WIN_Y, SUB_WIN_X, (MAIN_WIN_Y - SUB_WIN_Y) / 2,
	                                  (MAIN_WIN_X - SUB_WIN_X) / 2);
	tetris->w_game = init_window(MAIN_WIN_Y + 2, MAIN_WIN_X + 2, 0, 0);
	tetris->w_main = init_window(MAIN_WIN_Y + 2, MAIN_WIN_X + 2, 0, 0);
	return tetris->w_help && tetris->w_game_over && tetris->w_game && tetris->w_main;
}

/**
 * Draws the game menu together with the cursor on the currently selected item.
 * @param tetris
 * @param menuitem the currently selected item.
 */
void dw_draw_main_menu(tt_tetris *tetris, cursor_main_menu menuitem) {
	char menu[NUM_MAIN_MENU][11] = { "New Game", "High Score", "Help menu", "Quit" };
	wclear(tetris->w_main);
	box(tetris->w_main, 0, 0);
	mvwprintw(tetris->w_main, 0, MAIN_WIN_X / 2 - 7, "[ Main Menu ]");
	for (int i = 0; i < NUM_MAIN_MENU; i++) {
		if (i == menuitem) {
			mvwaddstr(tetris->w_main, MAIN_WIN_Y / 6 + i, MAIN_WIN_X / 6 - 3, "-> ");
			wattron(tetris->w_main, A_REVERSE);
		}
		mvwaddstr(tetris->w_main, MAIN_WIN_Y / 6 + i, MAIN_WIN_X / 6, menu[i]);
		wattroff(tetris->w_main, A_REVERSE);
	}
	mvwaddstr(tetris->w_main, 5 * MAIN_WIN_Y / 6, MAIN_WIN_X / 6,
	          "Use arrow keys to move! Press ENTER to select!");
	wrefresh(tetris->w_main);
}

/**
 * Draws the game over window together with the score reached this round.
 * @param tetris
 */
void dw_draw_game_over(tt_tetris *tetris) {
	wclear(tetris->w_game_over);
	box(tetris->w_game_over, 0, 0);
	mvwprintw(tetris->w_game_over, 0, SUB_WIN_X / 2 - 7, "[ Game Over ]");
	mvwaddstr(tetris->w_game_over, 8, 3, "Press any key to restart!");
	mvwprintw(tetris->w_game_over, 5, 3, "Score: %3d", tetris->score);
}

/**
 * Draws the game window together with the board and the tetris block currently
 * falling.
 * @param tetris
 */
void dw_draw_game_window(tt_tetris *tetris) {
	wclear(tetris->w_game);
	box(tetris->w_game, 0, 0);
	mvwprintw(tetris->w_game, 0, MAIN_WIN_X / 2 - 9, "[ Terminal-Tetris ]");
	mvwprintw(tetris->w_game, MAIN_WIN_Y + 1, MAIN_WIN_X - 15, "[ Score: %3d ]", tetris->score);

	int gameing_area_x = MAIN_WIN_X / 2 - BOARD_X + 2; // 31
	int gameing_area_y = MAIN_WIN_Y / 6; // 5
	
	// draw board
	for (int y = 0; y < BOARD_Y; y++) { // "<|| - - - - - - - - - - - ||>"
		mvwprintw(tetris->w_game, y + gameing_area_y, gameing_area_x - 4, "<|| ");
		mvwprintw(tetris->w_game, y + gameing_area_y, gameing_area_x + 21, " ||>");
		for (int x = 0; x < BOARD_X; x++) {
			if (tetris->board[y][x]) {
				mvwprintw(tetris->w_game, gameing_area_y + y, gameing_area_x + x*2, "%c", CHAR_OCCUPIED);
			}
		}
	}
	mvwprintw(tetris->w_game, 20 + gameing_area_y, gameing_area_x - 4, "<|| = = = = = = = = = = = ||>");
	mvwprintw(tetris->w_game, 21 + gameing_area_y, gameing_area_x, "V V V V V V V V V V V    ");
	mvwprintw(tetris->w_game, 22 + gameing_area_y, gameing_area_x, "x: %d", tetris->current_block.x);
	mvwprintw(tetris->w_game, 23 + gameing_area_y, gameing_area_x - 10, "y: %d", tetris->current_block.y);
	
	// draw next block display borders
	mvwprintw(tetris->w_game, gameing_area_y, gameing_area_x - 15, "=========");
	mvwprintw(tetris->w_game, gameing_area_y + 5, gameing_area_x - 15, "=========");
	for (int i = 1; i < 5; i++) {
		mvwprintw(tetris->w_game, gameing_area_y + i, gameing_area_x - 15, "|");
		mvwprintw(tetris->w_game, gameing_area_y + i, gameing_area_x - 7, "|");
	}
	
	// draw next block
	for (int i = 0; i < tetris->next_block.width; i++) {
		for (int j = 0; j < tetris->next_block.width; j++) {
			if (tetris->next_block.array[i][j]) {
				mvwprintw(tetris->w_game, i + gameing_area_y+1, gameing_area_x - 13 + j*2, "%c", CHAR_OCCUPIED);
			}
		}
	}
	
	// draw current block to the board
	for (int y = 0; y < tetris->current_block.width; ++y) {
		for (int x = 0; x < tetris->current_block.width; ++x) {
			int fx = tetris->current_block.x + x;
			int fy = tetris->current_block.y + y;

			if (tetris->current_block.array[y][x]) {
				mvwprintw(tetris->w_game, fy + gameing_area_y, 2 * fx + gameing_area_x, "%c ", CHAR_OCCUPIED);
			}
		}
	}
	wrefresh(tetris->w_game);
	refresh();
}

/**
 * Function that acts similar to a popup in any browser.
 * It draws the window w_next over the current window.
 * As soon as any button has been pressed window w_next_next will be drawn.
 * @param tetris
 */
void dw_show_static_window(WINDOW *w_next, WINDOW *w_next_next) {
	touchwin(w_next);
	wrefresh(w_next);
	getchar();
	touchwin(w_next_next);
	wrefresh(w_next_next);
}

/**
 * Frees all windows from memory and restores the normal terminal settings,
 * manipulated by ncurses.
 * @param tetris
 */
void dw_delete_windows(tt_tetris *tetris) {
	delwin(tetris->w_help);
	delwin(tetris->w_game_over);
	delwin(tetris->w_game);
	delwin(tetris->w_main);
	endwin();
}
