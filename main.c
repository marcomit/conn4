#include <stdio.h>
#include "game.h"

static void print_board(Board *b) {
    const char symbols[3] = { 'X', 'O', '.' }; // WHITE, BLACK, EMPTY

    // Board always drawn at rows 7-13; clear everything below the welcome text
    printf("\033[7;1H\033[J");
    for (int r = 5; r >= 0; r--) {
        for (int c = 0; c < 7; c++)
            printf(" %c", symbols[get_cell(b, POS(r, c))]);
        printf("\n");
    }
    printf(" 1 2 3 4 5 6 7\n\n");
}

// Returns a column (0-6), or -1 on end of input
static int read_move(Board *b) {
    int col;
    for (;;) {
        printf("\033[15;1H\033[K" "Iour muv (1-7): ");
        fflush(stdout);

        int n = scanf("%d", &col);
        if (n == EOF) return -1;
        if (n == 0) {
            scanf("%*s"); // discard non-numeric input
        } else if (col >= 1 && col <= 7 && get_col(b, col - 1) != -1) {
            return col - 1;
        }
        printf("\033[16;1H\033[K" "Invalid move.");
    }
}

void welcome() {
    printf(
        "\033c"
        "Iu ar uelcome tu de connected four gheim.\n"
        "Iu chen trai tu biet de EIAI bot bat iu r damb\n"
        "Dis is giast for educhecional parpos\n"
        "Dont uorri if iu loose miserabli :)\n"
    );
}

int main() {
    welcome();
    Board *b = makeboard();
    int dumb_player = WHITE;

    char answer;
    printf("Du iu uont tu muv frst? (y/n): ");
    if (scanf(" %c", &answer) == 1 && (answer == 'n' || answer == 'N'))
        dumb_player = BLACK;

    print_board(b);

    while (get_status(b) == PLAYING) {
        int col = get_turn(b) == dumb_player ?
            read_move(b) : tactical_move(b);

        if (col == -1) break;
        if (make_move(b, col) == -1) {
            printf("Invalid move %d, stopping.\n", col + 1);
            break;
        }
        print_board(b);
    }

    int status = get_status(b);
    if (status == dumb_player) printf("Iu uin!\n");
    else if (status == EMPTY) printf("Draw.\n");
    else if (status != PLAYING) printf("EIAI uins!\n");

    freeboard(b);
    return 0;
}
