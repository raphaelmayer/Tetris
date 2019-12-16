#include "tt_game.h"

/**
 * Adds the currently falling block to the board.
 * The pixel position of the block will only change when a line is full from now on.
 * @param tetris
 */
static void add_block_to_board(tt_tetris *tetris) {
	int len = tetris->current_block.width;
	int offset_y = BOARD_Y - tetris->current_block.y - len; // for empty rows, when reaching ground
	for (int i = 0; i < len; i++) {
		for (int j = 0; j < len; j++) {
			if (tetris->current_block.array[i][j]) {
				tetris->board[BOARD_Y - len + i - offset_y][tetris->current_block.x + j] = 'O';		
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

tetris_block blocks[] = { 
	(tetris_block){ 2, 0, 0, {{ 1, 1 }, { 1, 1 }} }, // O
	(tetris_block){ 3, 0, 0, {{ 0, 1, 0 }, { 0, 1, 0 }, { 1, 1, 0 }} }, // J
	(tetris_block){ 3, 0, 0, {{ 0, 1, 0 }, { 0, 1, 0 }, { 0, 1, 1 }} }, // L
	(tetris_block){ 3, 0, 0, {{ 0, 1, 0 }, { 1, 1, 1 }, { 0, 0, 0 }} }, // T
	(tetris_block){ 4, 0, 0, {{ 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 }} }, // I
	(tetris_block){ 3, 0, 0, {{ 0, 1, 1 }, { 1, 1, 0 }, { 0, 0, 0 }} }, // S
	(tetris_block){ 3, 0, 0, {{ 1, 1, 0 }, { 0, 1, 1 }, { 0, 0, 0 }} } // Z
};

static void new_block(tt_tetris *tetris) {
	int rnd = rand() % 7;
	tetris->current_block = blocks[rnd];
	reset_block(tetris);
	tetris->next_block = blocks[rnd];
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
			if (block.array[i][offset]) return 0;
	}
	if (dir == 'D') { // down
		int offset = BOARD_Y - block.y - 1;
		for (int i = 0; i < len; i++)
			if (block.array[offset][i]) return 0;
	}
	if (dir == 'L') { // left
		int offset = block.x * (-1);
		for (int i = 0; i < len; i++)
			if (block.array[i][offset]) return 0;
	}
	if (dir == 'S') { // spin / rotate
		if (BOARD_X <= block.x + len) { // near right
			int offset = BOARD_X - block.x;
			for (int i = 0; i < len; i++)
				if (block.array[i][offset]) return 0;
		} else if (block.x < block.width-1) { // near left
			int offset = block.x * (-1);
			for (int i = 0; i < len; i++)
				if (block.array[i][offset]) return 0;
		} else { // near bottom
			int offset = BOARD_Y - block.y - 1;
			for (int i = 0; i < len; i++)
				if (block.array[offset][i]) return 0;
		}
	}
	return 1;
}

// function to rotate the matrix 90 degree clockwise 
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

static bool valid_move(tt_tetris *tetris, int x_move, int y_move) {
	int len = tetris->current_block.width;

	if (x_move && x_move < 0) { // move left
		return 0 < tetris->current_block.x || check_bounds(tetris->current_block, 'L');
	}	
	if (x_move && 0 < x_move) { // move right
		return tetris->current_block.x + len < BOARD_X || check_bounds(tetris->current_block, 'R');
	}	
	if (!y_move && !x_move) { // rotation
		return (
			0 < tetris->current_block.x + 1 
			&& tetris->current_block.x + len < BOARD_X 
			&& tetris->current_block.y + len <= BOARD_Y
		) || check_bounds(rotate_block(tetris->current_block), 'S');
	}
	// move down
	return (tetris->current_block.y + 2 + y_move < 23 - len && !x_move) || check_bounds(tetris->current_block, 'D');
}

/**
 * Function that defines the behavior of a falling block.
 * If the block reaches the floor or an occupied pixel below,
 * it will be added to the board to a static / non moving element.
 * @param tetris
 * @return
 */
static bool try_vertical_move(tt_tetris *tetris) {
	bool valid = valid_move(tetris, 0, 1);
	if (valid) {
		++tetris->current_block.y;
	} else {
		add_block_to_board(tetris);
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
	return false;
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
		// case TT_FALL_DOWN: try_vertical_move(tetris); break;
		case TT_ROTATE: try_rotation(tetris); break;
		case TT_DOWN: try_vertical_move(tetris); break;
		default: break;
	}
}

/**
 * Resets all parameters of the tt_tetris struct to a new game state.
 * This includes resetting the score to zero or clearing the tetris board.
 * @param tetris
 */
void gm_reset_game(tt_tetris *tetris) {
	tetris->score = 0;
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
	tetris->speed = INIT_SPEED;
}
