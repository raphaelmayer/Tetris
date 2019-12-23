#include "tt_game.h"
#include "tt_types.h"
#include "tt_score.h"

/**
 * Adds the currently falling block to the board.
 * The pixel position of the block will only change when a line is full from now on.
 * @param tetris
 */
static void add_block_to_board(tt_tetris *tetris) {
	tetris_block block = tetris->current_block;
	int len = block.width;
	for (int i = 0; i < len; i++) {
		for (int j = 0; j < len; j++) {
			if (block.array[i][j]) {
				// block.color is a nr between 1 and 7, which is true,
				// so it still works like before, where 1 got assigned for full tiles.
				tetris->board[block.y + i][block.x + j] = block.color;
			}
		}
	}
}

/**
 * Moves the current block to its initial starting position from which it starts falling.
 * @param tetris
 */
static void reset_block(tt_tetris *tetris) {
	tetris->current_block.x = (BOARD_X - tetris->current_block.width) / 2;
	tetris->current_block.y = 0;
}

/**
 * Creates a new randomly selected block for the block preview (next_block),
 * while the currently shown block becomes the new block, which to be controlled by the player
 * (current_block).
 * @param tetris
 */
static void new_block(tt_tetris *tetris) {
	tetris_block blocks[] = { 
		(tetris_block){ 2, 0, 0, {{ 1, 1 }, { 1, 1 }}, O_BLOCK },
		(tetris_block){ 3, 0, 0, {{ 1, 0, 0 }, { 1, 1, 1 }, { 0, 0, 0 }}, J_BLOCK },
		(tetris_block){ 3, 0, 0, {{ 0, 0, 1 }, { 1, 1, 1 }, { 0, 0, 0 }}, L_BLOCK },
		(tetris_block){ 3, 0, 0, {{ 0, 1, 0 }, { 1, 1, 1 }, { 0, 0, 0 }}, T_BLOCK },
		(tetris_block){ 4, 0, 0, {{ 0, 0, 0, 0 }, { 1, 1, 1, 1 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }}, I_BLOCK },
		(tetris_block){ 3, 0, 0, {{ 0, 1, 1 }, { 1, 1, 0 }, { 0, 0, 0 }}, S_BLOCK },
		(tetris_block){ 3, 0, 0, {{ 1, 1, 0 }, { 0, 1, 1 }, { 0, 0, 0 }}, Z_BLOCK }
	};
	int rnd = rand() % 7;
	tetris->current_block = tetris->next_block;
	reset_block(tetris);
	tetris->next_block = blocks[rnd]; // 4
	tetris->speed *= .95; // increase game speed with each new block
	++tetris->block_count;
}

// function to rotate a block by 90 degrees clockwise 
tetris_block rotate_block(tetris_block block) { 
	int len = block.width;
    for (int i = 0; i < len / 2; i++) { 
        for (int j = i; j < len - i - 1; j++) { 
            int temp = block.array[i][j]; 
            block.array[i][j] = block.array[len-1 - j][i]; 
            block.array[len-1 - j][i] = block.array[len-1 - i][len-1 - j]; 
            block.array[len-1 - i][len-1 - j] = block.array[j][len-1 - i]; 
            block.array[j][len-1 - i] = temp; 
        } 
    } 
	return block;
} 

/**
 * Helper function that returns true, if any tile of the given block's bounding box 
 * is not in the game area. Takes offset to adjust which tile or to test a move.
 * bounding box: square, that contains the tetromino
 * @param block
 * @param x_offset
 * @param y_offset
 */
bool is_out_of_bounds(tetris_block block, int x_offset, int y_offset) {
	return (
		block.y + y_offset < 0 ||
		block.x + x_offset < 0 ||
		block.y + y_offset >= BOARD_Y ||
		block.x + x_offset >= BOARD_X
	);
}

/**
 * Before any movement of a block is allowed, collision detection has to be performed.
 * Therefore the new block position, which is calculated by the current position plus the offset
 * [x_move, y_move] given, has to be checked for overlaps with possible occupied pixels.
 * Returns true, if the current block would collide with any other block or any bound.
 * @param tetris
 * @param x_move
 * @param y_move
 * @param rotation
 * @return
 */ 
bool would_collide(tetris_block block, char board[BOARD_Y][BOARD_X], int x_move, int y_move) {
	int len = block.width;
	// loops through all tiles of a block
	for (int i = 0; i < len; i++) {
		for (int j = 0; j < len; j++) {
			// checks all tiles of the board where the block would land and 
			// the corresponding tile of the block. if both are 1 => collision
			if (block.array[j][i] && board[block.y + y_move + j][block.x + x_move + i]) return true;
			// or if corresponding tiles are 1 and out of bounds => collision
			if (block.array[j][i] && is_out_of_bounds(block, x_move + i, y_move + j)) return true;
		}
	}
	return false;
}

static bool valid_move(tt_tetris *tetris, int x_move, int y_move, bool rotation) {
	// if no moves, it is rotation => block needs to be rotated before checking collision
	tetris_block block = rotation ? rotate_block(tetris->current_block) : tetris->current_block;
	return !would_collide(block, tetris->board, x_move, y_move);
}

/**
 * Helper function that returns true, if the given row is full and to be cleared.
 * Argument row is the y coordinate of the current block.
 * @param tetris
 * @param row
 */
bool is_row_full(tt_tetris *tetris, int row) {
	for (int i = 0; i < BOARD_X; i++) {
		if (!tetris->board[row][i]) return false;
	}
	return true;
}

/**
 * Function that actually clears a single row and moves the rows above down.
 * Argument row is the y coordinate of the current block.
 * @param tetris
 * @param row
 */
void clear_row_and_move_rows_above(tt_tetris *tetris, int row) {
	for (int i = 0; i < BOARD_X; i++) {
		for (int j = row; j >= 0; j--) {
			// the second loop moves the blocks above down
			// the top row will always be zero
			tetris->board[j][i] = j ? tetris->board[j - 1][i] : 0;
		}
	}
}

// not in use
void clear_row(tt_tetris *tetris, int row) {
	for (int i = 0; i < BOARD_X; i++) {
		tetris->board[row][i] = 0;
	}
}
// not in use
void move_rows_down(tt_tetris *tetris, unsigned row_count) {
	int len = tetris->current_block.width;
	int row = tetris->current_block.y + (len-1) < BOARD_Y ? tetris->current_block.y + (len-1) : 0;
	for (int i = 0; i < BOARD_X; i++) {
		for (int j = row; j >= 0; j--) {
			// the second loop moves the blocks above down
			// the top row will always be zero
			tetris->board[j][i] = tetris->board[j - row_count][i] ? tetris->board[j - row_count][i] : 0;
		}
	}
}

/**
 * Function that clears full rows and, accumulates the number of rows cleared.
 * Also applies a multiplier, if multiple rows are cleared simultaneously.
 * Rows get cleared one by one and after each clear the rows above will be moved one tile down.
 * Inefficient when multiple rows get cleared in one go. 
 * @param tetris
 */
void delete_lines(tt_tetris *tetris) {
	tetris_block block = tetris->current_block;
	unsigned row_count = 0;
	for (int row = block.y; row < block.y + block.width; row++) {
		if (is_row_full(tetris, row)) {
			clear_row_and_move_rows_above(tetris, row);
			++row_count;
		}
	}
	// if (row_count) move_rows_down(tetris, row_count);
	tetris->score += (row_count * 10) * row_count;
}

/**
 * Function that defines the behavior of a falling block.
 * If the block reaches the floor or an occupied pixel below,
 * it will be added to the board to a static / non moving element.
 * @param tetris
 * @return
 */
static bool try_vertical_move(tt_tetris *tetris, enum tt_movement move) {
	bool valid = valid_move(tetris, 0, 1, false);
	if (valid) {
		++tetris->current_block.y;

		// recursively implement the fall down mechanic
		if (move == TT_FALL_DOWN) {
			try_vertical_move(tetris, TT_FALL_DOWN);
		}
	} else {
		add_block_to_board(tetris);
		delete_lines(tetris);
		new_block(tetris);
	}
	return valid;
}
static bool try_horizontal_move(tt_tetris *tetris, enum tt_movement move) {
	int dir = move == TT_LEFT ? -1 : 1; // direction: left or right
	bool valid = valid_move(tetris, dir, 0, false);
	if (valid) {
		tetris->current_block.x += dir;
	}
	return valid;
}
static bool try_rotation(tt_tetris *tetris) {
	// if the regular rotation collided it tries,
	// if a rotation could still happen by offsetting the block by 1 to the left or right
	// I-block sometimes should be offset by 2 to rotate, which is not implemented :(
	bool valid = valid_move(tetris, 0, 0, true);
	bool left_is_valid = valid_move(tetris, -1, 0, true);
	bool right_is_valid = valid_move(tetris, 1, 0, true);
	if (valid || left_is_valid || right_is_valid) {
		tetris->current_block = rotate_block(tetris->current_block);
	}
	if (!valid && left_is_valid) {
		tetris->current_block.x -= 1;
	} else if (!valid && right_is_valid) {
		tetris->current_block.x += 1;
	}
	return valid || left_is_valid || right_is_valid;
}
static bool try_alter_time(tt_tetris *tetris) {
	tetris->speed = 500000;
	return true;
}


/**
 * Removes all occupied pixels of the board for a new / fresh game.
 * @param tetris
 */
static void clear_board(tt_tetris *tetris) {
	memset(tetris->board, 0, sizeof(**tetris->board) * BOARD_Y * BOARD_X);
}

/**
 * Checks if any game over condition is satisfied.
 * Generally this is true when a block is not able to fall anymore, even though it just has been
 * spawned.
 * @param tetris
 * @return a bool that is true if any game over condition is valid.
 */
bool gm_is_game_over(tt_tetris *tetris) {
	bool is_over = (tetris->current_block.y < 1) && !valid_move(tetris, 0, 1, false);
	if (is_over) update_highscores(tetris->score);
	return is_over;
}

/**
 * A function that is called, if a key event occurs that should move or rotate a block in a certain
 * way.
 * @param tetris
 * @param move informs about the key event that should be covered by a certain move of a block.
 */
void gm_move_block(tt_tetris *tetris, enum tt_movement move) {
	switch (move) {
		case TT_LEFT: try_horizontal_move(tetris, TT_LEFT); break;
		case TT_RIGHT: try_horizontal_move(tetris, TT_RIGHT); break;
		case TT_FALL_DOWN: try_vertical_move(tetris, TT_FALL_DOWN); break;
		case TT_ROTATE: try_rotation(tetris); break;
		case TT_DOWN: try_vertical_move(tetris, TT_DOWN); break;
		case TT_ALTER_TIME: try_alter_time(tetris); break;
		default: break;
	}
}

/**
 * Resets all parameters of the tt_tetris struct to a new game state.
 * This includes resetting the score to zero or clearing the tetris board.
 * @param tetris
 */
void gm_reset_game(tt_tetris *tetris) {
	tetris->speed = INIT_SPEED;
	tetris->score = 0;
	tetris->block_count = 0;
	clear_board(tetris);
	reset_block(tetris);
}

/**
 * Initializes the game with a new preview and falling block.
 * It also sets the initial falling speed of the block and performs preparation for the tetris
 * board.
 * @param tetris
 */
void gm_init_game(tt_tetris *tetris) {
	new_block(tetris);
	new_block(tetris);
	gm_reset_game(tetris);
}
