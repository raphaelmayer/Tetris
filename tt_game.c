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

/**
 * Before any movement of a block is allowed, collision detection has to be performed.
 * Therefore the new block position, which is calculated by the current position plus the offset
 * [x_move, y_move] given, has to be checked for overlaps with possible occupied pixels.
 * @param tetris
 * @param x_move
 * @param y_move
 * @return
 */
bool check_bounds(tetris_block block, char dir) {
	int len = block.width;
	// dir...direction: R, L, D, S
	if (dir == 'R') { //right
		int offset = BOARD_X - block.x - 1;
		for (int i = 0; i < len; i++)
			if (block.array[i][offset]) return false;
	}
	if (dir == 'D') { // down
		int offset = BOARD_Y - block.y - 1;
		for (int i = 0; i < len; i++)
			if (block.array[offset][i]) return false;
	}
	if (dir == 'L') { // left
		int offset = block.x * (-1);
		for (int i = 0; i < len; i++)
			if (block.array[i][offset]) return false;
	}
	if (dir == 'S') { // spin / rotate
		if (BOARD_X <= block.x + len) { // near right
			int offset = BOARD_X - block.x;
			for (int i = 0; i < len; i++)
				if (block.array[i][offset]) return false;
		} else if (block.x < block.width-1) { // near left
			int offset = block.x * (-1);
			for (int i = 0; i < len; i++)
				if (block.array[i][offset]) return false;
		} else { // near bottom
			int offset = BOARD_Y - block.y - 1;
			for (int i = 0; i < len; i++)
				if (block.array[offset][i]) return false;
		}
	}
	return true;
}

// function to rotate a block by 90 degrees clockwise 
tetris_block rotate_block(tetris_block block) { 
	// tetris_block block = tetris->current_block;
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

// returns true, if the current block would collide with any other 
// already set blocks when doing the proposed move
bool would_collide(tt_tetris *tetris, int x_move, int y_move) {
	tetris_block block = tetris->current_block;
	int len = block.width;
	for (int i = 0; i < len; i++) {
		for (int j = 0; j < len; j++) {
			if (tetris->board[block.y+len-1-j + y_move][block.x+i + x_move] && block.array[len-1-j][i]) return true;
		}
	}
	return false;
}
// returns true, if the complete bounding box of a block
// is within the game area (bounding box: square, that contains tetromino)
bool bounding_box_within_gamearea(tetris_block block) {
	int len = block.width;
	return (
		block.x >= 0 && 
		block.x + len <= BOARD_X && 
		block.y + len <= BOARD_Y
	);
}
static bool valid_move(tt_tetris *tetris, int x_move, int y_move) {
	if (would_collide(tetris, x_move, y_move)) return false;
	
	tetris_block block = tetris->current_block;
	int len = block.width;

	if (x_move < 0) { // left
		return 0 < block.x || check_bounds(block, 'L');
	}	
	if (x_move > 0) { // right
		return block.x + len < BOARD_X || check_bounds(block, 'R');
	}	
	if (!y_move && !x_move) { // rotation
		return bounding_box_within_gamearea(block) || check_bounds(rotate_block(block), 'S');
	}
	// down
	return (block.y + len < BOARD_Y) || check_bounds(block, 'D');
}

/**
 * returns true, if the given row is full and to be cleared.
 * param row is the y coordinate of the current block.
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
 * Function that actually clears a row and moves the rows above down.
 * param row is the y coordinate of the current block.
 * @param tetris
 * @param row
 */
void clear_row(tt_tetris *tetris, int row) {
	for (int i = 0; i < BOARD_X; i++) {
		for (int j = 0; j < row; j++) {
			// the second loop moves the blocks above down
			// if we are in the top row, eg. row = 0 
			// they will always be zero
			if (row - j) {
				tetris->board[row - j][i] = tetris->board[row - j - 1][i];
			} else tetris->board[row - j][i] = 0;
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
