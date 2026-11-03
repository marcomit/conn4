#ifndef GAME_H

#define GAME_H

#define WHITE 0
#define BLACK 1
#define EMPTY 2
#define PLAYING 3

#define file(p) ((int)(p) % 7)
#define rank(p) ((int)(p) / 7)

#define POS(r, f) ((r) * 7 + (f))

typedef struct Board Board;
typedef int Position;

Board *makeboard();

// -1 for invalid moves
int make_move(Board *, int);
void undo_move(Board *, Position);

void freeboard(Board *);

int get_turn(Board *);
int get_status(Board *);
int get_col(Board *, int);
int get_cell(Board *, Position);
void set_cell(Board *, Position, int);

int tactical_move(Board *);

#endif
