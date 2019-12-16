#ifndef TT_GAME_H
#define TT_GAME_H

#include "tt_types.h"

/**
 * Checks if any game over condition is satisfied.
 * Generally this is true when a block is not able to fall anymore, even though it just has been
 * spawned.
 * @param tetris
 * @return a bool that is true if any game over condition is valid.
 */
bool gm_is_game_over(tt_tetris *tetris);

/**
 * A function that is called, if a key event occurs that should move or rotate a block in a certain
 * way.
 * @param tetris
 * @param move informs about the key event that should be covered by a certain move of a block.
 */
void gm_move_block(tt_tetris *tetris, enum tt_movement move);

/**
 * Resets all parameters of the tt_tetris struct to a new game state.
 * This includes resetting the score to zero or clearing the tetris board.
 * @param tetris
 */
void gm_reset_game(tt_tetris *tetris);

/**
 * Initializes the game with a new preview and falling block.
 * It also sets the initial falling speed of the block and performs preparation for the tetris
 * board.
 * @param tetris
 */
void gm_init_game(tt_tetris *tetris);

#endif // TT_GAME_H
