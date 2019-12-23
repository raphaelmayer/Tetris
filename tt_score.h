#ifndef TT_score_H
#define TT_score_H

#include "tt_types.h"

/**
 * Reads the highscore list from a local file. If no such file exists,
 * it creates a default list by calling create_default_list();.
 * @return
 */
struct highscore* read_highscores();

/**
 * Checks, if a given score is higher than a current highscore and
 * inserts it, if true.
 * @param score int
 */
void update_highscores(int score);

#endif // TT_SCORE_H
