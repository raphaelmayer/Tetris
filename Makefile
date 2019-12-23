CFLAGS = -std=c99 -Wall -Werror
LDLIBS = -lncurses

.PHONY: all clean

all: main

clean:
	$(RM) main tt_tetris.o tt_game.o tt_draw.o tt_score.o

main: main.c tt_tetris.o tt_game.o tt_draw.o tt_score.o