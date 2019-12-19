## Tetris in C
A simple, text-based Tetris game.

#### Install
Install via Makefile:
```
git clone
cd Tetris
make && ./main
```

##### *to do*: 
- clear full rows (done, aber wies funzt im edgecase ganz oben..ka)
- background? ('-')

- color: 
    - draw.c: uncomment line 132, 134 (wattron, wattroff)
    - tetris.c: uncomment line 10 (start_color())
- slow motion
- preview where block lands
- hold block