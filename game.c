#include "game.h"
#include <stdint.h>
#include <stdlib.h>

typedef uint64_t bitboard;
#define BIT(p) ((bitboard)1ULL << (p))
#define FULL_BOARD_MASK 0x3FFFFFFFFFFULL
// Starting cells of a 4-in-a-row: files 0-3 (rightward) and files 3-6 (leftward)
#define FILES_0_3 0x78F1E3C78FULL
#define FILES_3_6 0x3C78F1E3C78ULL

struct Board {
    bitboard white;
    bitboard black;
    int turn;
};

Board *makeboard() {
    Board *self = (Board *)malloc(sizeof(Board));
    if (!self) return NULL;
    self->black = 0;
    self->white = 0;
    self->turn = WHITE;
    return self;
}

void freeboard(Board *b) { 
    free(b); 
}

int get_cell(Board *b, Position p) {
    if (b->black & BIT(p)) return BLACK;
    if (b->white & BIT(p)) return WHITE;
    return EMPTY;
}

void set_cell(Board *b, Position p, int val) {
    switch (val) {
    case WHITE:
        b->white |= BIT(p);
        break;
    case BLACK:
        b->black |= BIT(p);
        break;
    case EMPTY:
        b->white &= ~BIT(p);
        b->black &= ~BIT(p);
        break;
    default: break;
    }
}

int get_col(Board *b, int col) {
    if (col < 0 || col >= 7) return -1;
    for (int r = 0; r < 6; r++) {
        if (get_cell(b, POS(r, col)) == EMPTY) {
            return r;
        }
    }
    return -1; // Full column
}

int is_draw(Board *b) {
    return ((b->white | b->black) & FULL_BOARD_MASK) == FULL_BOARD_MASK;
}

int check_win(bitboard b) {
    // Shifts for 7x6 grid
    bitboard v  = b & (b >> 7);
    if (v & (v >> 14)) return 1;

    // Rows have no padding bits, so mask the final result to drop
    // lines that wrap from one row into the next
    bitboard h  = b & (b >> 1);
    if (h & (h >> 2) & FILES_0_3) return 1;

    bitboard d1 = b & (b >> 6);
    if (d1 & (d1 >> 12) & FILES_3_6) return 1;

    bitboard d2 = b & (b >> 8);
    if (d2 & (d2 >> 16) & FILES_0_3) return 1;

    return 0;
}

int get_status(Board *b) {
    if (check_win(b->white)) return WHITE;
    if (check_win(b->black)) return BLACK;
    if (is_draw(b)) return EMPTY;
    return PLAYING;
}

int get_turn(Board *b) { return b->turn; }

// Pass column (0-6) into move execution
int make_move(Board *b, int col) {
    int r = get_col(b, col);
    if (r == -1) return -1; // Invalid move (column full)

    Position p = POS(r, col);

    if (b->turn == WHITE) {
        b->white |= BIT(p);
        b->turn = BLACK;
    } else {
        b->black |= BIT(p);
        b->turn = WHITE;
    }

    return p; // Success
}

// Reverse a move returned by make_move
void undo_move(Board *b, Position p) {
    b->white &= ~BIT(p);
    b->black &= ~BIT(p);
    b->turn = b->turn == WHITE ? BLACK : WHITE;
}
