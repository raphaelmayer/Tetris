#include "tt_game.h"
#include "tt_types.h"

/**
 * Adds the currently falling block to the board.
 * The pixel position of the block will only change when a line is full from now on.
 * @param tetris
 */
static void add_block_to_board(tt_tetris *tetris) {
	int len = tetris->current_block.width;
	// int offset_y = BOARD_Y - tetris->current_block.y - len; // for empty rows, when reaching ground
	for (int i = 0; i < len; i++) {
		for (int j = 0; j < len; j++) {
			if (tetris->current_block.array[i][j]) {
				tetris->board[tetris->current_block.y + i][tetris->current_block.x + j] = 1;		
			}
		}
	}
}

/**
 * Moves the current block to its initial starting position from which it starts falling.
 * @param tetris
 */
static void reset_block(tt_tetris *tetris) {}

/**
 * Creates a new randomly selected block for the block preview (next_block),
 * while the currently shown block becomes the new block, which to be controlled by the player
 * (current_block).
 * @param tetris
 */
static void new_block(tt_tetris *tetris) {
	tetris_block blocks[] = { 
		(tetris_block){ 2, 0, 0, {{ 1, 1 }, { 1, 1 }} }, // O
		(tetris_block){ 3, 0, 0, {{ 0, 1, 0 }, { 0, 1, 0 }, { 1, 1, 0 }} }, // J
		(tetris_block){ 3, 0, 0, {{ 0, 1, 0 }, { 0, 1, 0 }, { 0, 1, 1 }} }, // L
		(tetris_block){ 3, 0, 0, {{ 0, 1, 0 }, { 1, 1, 1 }, { 0, 0, 0 }} }, // T
		(tetris_block){ 4, 0, 0, {{ 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 }} }, // I
		(tetris_block){ 3, 0, 0, {{ 0, 1, 1 }, { 1, 1, 0 }, { 0, 0, 0 }} }, // S
		(tetris_block){ 3, 0, 0, {{ 1, 1, 0 }, { 0, 1, 1 }, { 0, 0, 0 }} } // Z
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
 * Returns true, if the current block would collide with any other block.
 * @param tetris
 * @param x_move
 * @param y_move
 * @return
 */ 
bool would_collide(tt_tetris *tetris, int x_move, int y_move) {
	// if no moves, it is rotation => block needs to be rotated before checking collision
	tetris_block block = (x_move || y_move) ? tetris->current_block : rotate_block(tetris->current_block);
	int len = block.width;
	// loops through all tiles of a block
	for (int i = 0; i < len; i++) {
		for (int j = 0; j < len; j++) {
			// checks all tiles of the board where the block would land and 
			// the corresponding tile of the block. if both are 1 => collision
			if (block.array[j][i] && tetris->board[block.y + y_move + j][block.x + x_move + i]) return true;
			// or if corresponding tiles are 1 and out of bounds => collision
			if (block.array[j][i] && is_out_of_bounds(block, x_move + i, y_move + j)) return true;
		}
	}
	return false;
}

static bool valid_move(tt_tetris *tetris, int x_move, int y_move) {
	// return !would_collide(tetris, x_move, y_move);
	if (would_collide(tetris, x_move, y_move)) return false;
	return true;
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
 * param row is the y coordinate of the current block.
 * @param tetris
 * @param row
 */
void clear_row(tt_tetris *tetris, int row) {
	for (int i = 0; i < BOARD_X; i++) {
		for (int j = 0; j < row; j++) {
			// the second loop moves the blocks above down
			// the top row will always be zero
			tetris->board[row - j][i] = (row - j) ? tetris->board[row - j - 1][i] : 0;
		}
	}
}

/**
 * Function that clears full rows and, accumulates the number of rows cleared.
 * Also applies a multiplier, if multiple rows are cleared simultaneously.
 * @param tetris
 */
void delete_lines(tt_tetris *tetris) {
	unsigned row_count = 0;
	for (int row = tetris->current_block.y; row < BOARD_Y; row++) {
		if (is_row_full(tetris, row)) {
			clear_row(tetris, row);
			++row_count;
		}
	}
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
	bool valid = valid_move(tetris, 0, 1);
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
	bool valid = valid_move(tetris, dir, 0);
	if (valid) {
		tetris->current_block.x += dir;
	}
	return valid;
}
static bool try_rotation(tt_tetris *tetris) {
	bool valid = valid_move(tetris, 0, 0);
	if (valid) {
		tetris->current_block = rotate_block(tetris->current_block);
	}
	return valid;
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
	return (tetris->current_block.y < 2) && !valid_move(tetris, 0, 1);
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
