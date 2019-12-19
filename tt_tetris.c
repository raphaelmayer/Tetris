#include "tt_tetris.h"

/**
 * Initializes all windows and structs.
 * @return a new tt_tetris struct containing all information needed to execute other functions in
 * this program.
 */
tt_tetris *tt_init_tetris() {
	initscr();
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);
	refresh();
	nodelay(stdscr, TRUE);
	timeout(TIME_DELAY);
	keypad(stdscr, TRUE);
	noecho();

	tt_tetris *tetris = malloc(sizeof(*tetris));
	if (!tetris) {
		return NULL;
	}
	gm_init_game(tetris);
	if (!dw_init_windows(tetris)) {
		tt_destroy_tetris(tetris);
		return NULL;
	}
	return tetris;
}

/**
 * Final method to be called to clear up all windows and structs dynamically allocated.
 * @param tetris
 */
void tt_destroy_tetris(tt_tetris *tetris) {
	dw_delete_windows(tetris);
	free(tetris);
}
