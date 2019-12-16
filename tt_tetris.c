#include "tt_tetris.h"

/**
 * Initializes all windows and structs.
 * @return a new tt_tetris struct containing all information needed to execute other functions in
 * this program.
 */
tt_tetris *tt_init_tetris() {
	initscr();
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
