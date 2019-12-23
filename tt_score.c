#include <stdio.h>
#include <stdlib.h>
#include "tt_types.h"

#define MAX 10

highscore *create_default_list() {
	static highscore default_highscores[MAX] = {
		(highscore){ "NA", 0 },
		(highscore){ "NA", 0 },
		(highscore){ "NA", 0 },
		(highscore){ "NA", 0 },
		(highscore){ "NA", 0 },
		(highscore){ "NA", 0 },
		(highscore){ "NA", 0 },
		(highscore){ "NA", 0 },
		(highscore){ "NA", 0 },
		(highscore){ "NA", 0 }
	};
	FILE *file = fopen("./highscores.txt", "wb");
	fwrite(default_highscores, sizeof(highscore), MAX, file);
	fclose(file);
	return default_highscores;
}

/**
 * Reads the highscore list from a local file. If no such file exists,
 * it creates a default list by calling create_default_list();.
 * @return
 */
highscore *read_highscores() {
	static highscore highscores[MAX];
	FILE *file = fopen("./highscores.txt", "rb");
	if (file == NULL) {
		printf("No file found!\n");
		return create_default_list();
	}
    fread(highscores, sizeof(highscore), MAX, file);
    fclose(file);
	return highscores;
}

/**
 * Checks, if a given score is higher than a current highscore and
 * inserts it, if true.
 * @param score int
 */
void update_highscores(int score) {
	FILE *file;
	highscore *highscores = read_highscores();

	// traverse highscore list from highest to lowest score
	for (int i = 0; i < MAX; i++) {
		if (highscores[i].score < score) {
			// move lower scores down down
			for (int j = MAX-1; i < j; j--) {
				highscores[j] = highscores[j-1];
			}
			// insert new score with a name into arr
			highscores[i].score = score;
			printf("Enter your name: ");
			scanf("%s", highscores[i].name);

			// write array to file
			file = fopen("./highscores.txt", "wb");
			if (file == NULL) {
				printf("Error writing to file!\n");
				exit(1);
			}
			fwrite(highscores, sizeof(highscore), MAX, file);
			fclose(file);
			break; // meh
		}
	}
}