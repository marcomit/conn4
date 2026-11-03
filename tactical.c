#include "game.h"

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define SEARCH_DEPTH 9
#define WIN_SCORE 1000000.0
#define LOWEST_SCORE (-2 * WIN_SCORE)
#define HIGHEST_SCORE (2 * WIN_SCORE)

static int eval_window(int window[4], int player, int opponent) {
    // player, opponent, empty
    int cells[3] = { 0, 0, 0 };
    int score = 0;

    for (int i = 0; i < 4; i++) {
        if (window[i] == player) cells[0]++;
        else if (window[i] == opponent) cells[1]++;
        else cells[2]++;
    }

    if (cells[0] == 4) score += 10000;
    else if (cells[0] == 3 && cells[2] == 1) score += 5;
    else if (cells[0] == 2 && cells[2] == 2) score += 2;

    if (cells[1] == 3 && cells[2] == 1) score -= 80;
    return score;
}

static double eval_score(Board *b, int player) {
    int opponent = player == WHITE ? BLACK : WHITE;

    double score = 0;

    int center_col = 3, center_cnt = 0;
    for (int i = 0; i < 6; i++)
        if (get_cell(b, POS(i, center_col)) == player)
            center_cnt++;

    score += center_cnt * 3;

    for (int r = 0; r < 6; r++) {
        for (int c = 0; c < 4; c++) {
            int window[4] = {
                get_cell(b, POS(r, c)),
                get_cell(b, POS(r, c + 1)),
                get_cell(b, POS(r, c + 2)),
                get_cell(b, POS(r, c + 3))
            };
            score += eval_window(window, player, opponent);
        }
    }

    for (int c = 0; c < 7; c++) {
        for (int r = 0; r < 3; r++) {
            int window[4] = {
                get_cell(b, POS(r, c)),
                get_cell(b, POS(r + 1, c)),
                get_cell(b, POS(r + 2, c)),
                get_cell(b, POS(r + 3, c))
            };
            score += eval_window(window, player, opponent);
        }
    }

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 4; c++) {
            int window[4] = {
                get_cell(b, POS(r, c)),
                get_cell(b, POS(r + 1, c + 1)),
                get_cell(b, POS(r + 2, c + 2)),
                get_cell(b, POS(r + 3, c + 3))
            };
            score += eval_window(window, player, opponent);
        }
    }

    for (int r = 3; r < 6; r++) {
        for (int c = 0; c < 4; c++) {
            int window[4] = {
                get_cell(b, POS(r, c)),
                get_cell(b, POS(r - 1, c + 1)),
                get_cell(b, POS(r - 2, c + 2)),
                get_cell(b, POS(r - 3, c + 3))
            };
            score += eval_window(window, player, opponent);
        }
    }

    return score;
}

typedef struct {
    Board *b;
    int player;
} EvalContext;

static inline double eval_max(EvalContext *, int depth, double alpha, double beta);
static inline double eval_min(EvalContext *, int depth, double alpha, double beta);

static inline int eval_leaf(EvalContext *ctx, int depth, double *score) {
    int status = get_status(ctx->b);
    if (status == ctx->player) *score = WIN_SCORE + depth;
    else if (status == WHITE || status == BLACK) *score = -WIN_SCORE - depth;
    else if (status == EMPTY) *score = 0;
    else if (depth == 0) *score = eval_score(ctx->b, ctx->player);
    else return 0;
    return 1;
}

// visit central columns before.
static const int columns[7] = {3, 2, 4, 1, 5, 0, 6};

static inline double eval_max(EvalContext *ctx, int depth, double alpha, double beta) {
    double score;
    if (eval_leaf(ctx, depth, &score)) return score;

    score = -2 * WIN_SCORE;
    for (int i = 0; i < 7; i++) {
        Position p = make_move(ctx->b, columns[i]);
        if (p == -1) continue;
        double child = eval_min(ctx, depth - 1, alpha, beta);
        undo_move(ctx->b, p);

        score = max(score, child);
        if (score >= beta) return score;
        alpha = max(alpha, score);
    }
    return score;
}

static inline double eval_min(EvalContext *ctx, int depth, double alpha, double beta) {
    double score;
    if (eval_leaf(ctx, depth, &score)) return score;

    score = 2 * WIN_SCORE;
    for (int i = 0; i < 7; i++) {
        Position p = make_move(ctx->b, columns[i]);
        if (p == -1) continue;
        double child = eval_max(ctx, depth - 1, alpha, beta);
        undo_move(ctx->b, p);

        score = min(score, child);
        if (score <= alpha) return score;
        beta = min(beta, score);
    }
    return score;
}

int tactical_move(Board *b) {
    EvalContext ctx = (EvalContext){
        .b = b,
        .player = get_turn(b)
    };
    
    int col = -1;
    double max_score = LOWEST_SCORE;
    for (int i = 0; i < 7; i++) {
        Position pos = make_move(b, columns[i]);
        if (pos == -1) continue;
        double child = eval_min(&ctx, SEARCH_DEPTH - 1, max_score, HIGHEST_SCORE);
        undo_move(b, pos);
        if (child > max_score) {
            col = columns[i];
            max_score = child;
        }
    }
    return col;
}
