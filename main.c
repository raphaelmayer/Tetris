#include <ncurses.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "tt_tetris.h"

cursor_main_menu main_menu(tt_tetris *tetris, cursor_main_menu menuitem);

void game_menu(tt_tetris *tetris);

void game_input(tt_tetris *tetris, int key);

int main(void) {
	srand((unsigned int)time(NULL));

	tt_tetris *tetris = tt_init_tetris();

	cursor_main_menu cursor = NEW_GAME;
	while (cursor != QUIT) {
		switch (cursor = main_menu(tetris, cursor)) {
		case NEW_GAME: game_menu(tetris); break;
		case HIGH_SCORE: dw_show_static_window(tetris->w_highscore, tetris->w_main); break;
		case HELP_MENU: dw_show_static_window(tetris->w_help, tetris->w_main); break;
		default: break;
		}
	}
	tt_destroy_tetris(tetris);
	return EXIT_SUCCESS;
}

long elapsed_time(struct timeval start, struct timeval current) {
	return ((current.tv_sec - start.tv_sec) * 1000000) + (current.tv_usec - start.tv_usec);
}

/**
 * Function for navigating through the different menu options.
 * It loops till a menu item has been selected.
 * @param tetris
 * @param menuitem describes the currently selected menu item.
 * @return the selected menu item if enter has been pressed.
 */
cursor_main_menu main_menu(tt_tetris *tetris, cursor_main_menu menuitem) {
	int key;
	dw_draw_main_menu(tetris, menuitem);

	while (true) {
		key = getch();
		switch (key) {
		case KEY_DOWN: menuitem = (menuitem + 1) % NUM_MAIN_MENU; break;
		case KEY_UP: menuitem = (menuitem - 1) % NUM_MAIN_MENU; break;
		case 'h':
			dw_show_static_window(tetris->w_help, tetris->w_main);
			break;
			return HELP_MENU;
		case '\n': return menuitem;
		case 'q': return QUIT;
		default: break;
		}
		dw_draw_main_menu(tetris, menuitem);
	}
}

/**
 * Starts the game and loops till the game is over or quit has been pressed.
 * It calls the following external functions at the start:
 *  - gm_reset_game
 *  - dw_draw_game_window
 * It calls the following functions in a loop:
 *  - dw_is_game_over
 *  - game_input
 *  - dw_draw_game_window
 * It calls the following functions once at the end:
 *  - dw_draw_game_over
 *  - dw_show_static_window
 *  - game_menu
 * @param tetris
 */
void game_menu(tt_tetris *tetris) {
	gm_reset_game(tetris);
	dw_draw_game_window(tetris);

	struct timeval start, current;
	gettimeofday(&start, NULL);
	while (!gm_is_game_over(tetris)) {
		int key = getch();
		if (key != ERR) {
			game_input(tetris, key);
		}
		if (key == 'q') {
			return;
		}
		gettimeofday(&current, NULL);
		if (elapsed_time(start, current) > tetris->speed) {
			game_input(tetris, KEY_DOWN);
			gettimeofday(&start, NULL);
		}
		dw_draw_game_window(tetris);
	}
	dw_draw_game_over(tetris);
	dw_show_static_window(tetris->w_game_over, tetris->w_game_over);
	game_menu(tetris);
}

/**
 * Function that maps a pressed key to the correct block movement.
 * @param tetris
 * @param key that has been pressed.
 */
void game_input(tt_tetris *tetris, int key) {
	// if (tetris->is_over) {}
	switch (key) {
	case 'h': dw_show_static_window(tetris->w_help, tetris->w_game);
	case KEY_LEFT: gm_move_block(tetris, TT_LEFT); break;
	case KEY_RIGHT: gm_move_block(tetris, TT_RIGHT); break;
	case KEY_DOWN: gm_move_block(tetris, TT_DOWN); break;
	case ' ': gm_move_block(tetris, TT_FALL_DOWN); break;
	case KEY_UP: gm_move_block(tetris, TT_ROTATE); break;
	case 's': gm_move_block(tetris, TT_ALTER_TIME); break;
	default: break;
	}
}
