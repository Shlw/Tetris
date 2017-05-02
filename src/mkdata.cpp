#include "util.h"
#include "game.h"
#include "decide.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

GameBoard gb;
int turn;

void printInfo()
{
    printf("Round %d:  %d vs %d\n", turn, gb.elimTotal[0], gb.elimTotal[1]);
    printf("count:");
    for (int i = 0; i < 7; ++i)
        printf(" %d", gb.typeCountForColor[0][i]);
    printf("\ncount:");
    for (int i = 0; i < 7; ++i)
        printf(" %d", gb.typeCountForColor[1][i]);
    putchar('\n');
    putchar('\n');
    static char ans[20][26] = {};
    for (int i = 1; i <= 20; ++i)
        sprintf(ans[i - 1], "%2d", 21 - i);
    for (int i = 0; i < 20; ++i)
        ans[i][2] = ans[i][13] = ans[i][24] = '|';
    static const char mp[3] = {' ', '*', '+'};
    for (int i = 1; i <= 20; ++i)
        for (int j = 1; j <= 10; ++j)
            ans[20 - i][2 + j] = mp[gb.gridInfo[0][i][j]];
    for (int i = 1; i <= 20; ++i)
        for (int j = 1; j <= 10; ++j)
            ans[20 - i][13 + j] = mp[gb.gridInfo[1][i][j]];
    for (int i = 0; i < 20; ++i)
        puts(ans[i]);
    printf("  ");
    for (int i = 0; i < 23; ++i)
        putchar('-');
    printf("\n   ");
    for (int i = 1; i <= 10; ++i)
        printf("%d", i % 10);
    putchar('|');
    for (int i = 1; i <= 10; ++i)
        printf("%d", i % 10);
    putchar('\n');
    getchar();
}

void printStatus()
{
    auto board = gb.gridInfo[0];
    for (int i = 1; i <= MAPWIDTH; ++i) {
        int mh = MAPHEIGHT;
        while (board[mh][i] == 0)
            --mh;
        int hole = mh - 1;
        while (hole >= 0 && board[hole][i] != 0)
            --hole;
        int hd;
        if (hole == -1)
            hd = -1;
        else {
            hd = 1;
            while (board[hole - hd][i] == 0)
                ++hd;
        }
        printf("%d %d %d ", mh, hole, hd);
    }
    putchar('\n');
}

int main()
{
    gb.currBotColor = 0;
    gb.enemyColor = 1;
    //srand(getpid() * time(0));
    srand(0);
    turn = 1;
    int block = rand() % 7;
    while (gb.canPut(0, block)) {
        printStatus();

        ++gb.typeCountForColor[0][block];
        int fx, fy, fo;
        naive_place(gb, 0, block, evaluate2, fx, fy, fo);

        memcpy(gb.gridInfo[1], gb.gridInfo[0], sizeof(gb.gridInfo[0]));
        memcpy(gb.typeCountForColor[1], gb.typeCountForColor[0], sizeof(gb.typeCountForColor[0]));
        gb.enemyType = block;
        int b2;
        naive_jam(gb, evaluate2, b2);

        gb.place(0, block, fx, fy, fo);
        gb.eliminate(0);
        block = b2;
        ++turn;
    }
    printf("%d\n", turn);
    return 0;
}
