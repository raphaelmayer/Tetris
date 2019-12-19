#ifndef TT_TETRIS_H
#define TT_TETRIS_H

#include "tt_draw.h"
#include "tt_game.h"

/**
 * Initializes all windows and structs.
 * @return a new tt_tetris struct containing all information needed to execute other functions in
 * this program.
 */
tt_tetris *tt_init_tetris();

/**
 * Initializes all windows and structs.
 * @return a new tt_tetris struct containing all information needed to execute other functions in
 * this program.
 */
void tt_destroy_tetris(tt_tetris *tetris);

/**
 * Initializes colors on supported terminals.
 */
void enable_color();

#endif // TT_TETRIS_H
