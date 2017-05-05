#include "util.h"
#include "game.h"
#include "decide.h"
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <queue>
#include <utility>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
using namespace std;

GameBoard gb;
int turn;

void printInfo(GameBoard &gb)
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

int putables(Board a, int c[MAPHEIGHT + 2][MAPWIDTH + 2])
{
    static GameBoard gb2;
    memcpy(gb2.gridInfo[0], a.grid, sizeof(a.grid));
    int ans = 0;
    for (int i = 0; i < 7; ++i) {
        vector<Tetris> loc;
        gb2.getPlaces(0, i, loc);
        for (auto t : loc)
            for (int k = 0; k < 4; k++) {
                int tmpX = t.blockX + t.shape[t.orientation][2 * k];
                int tmpY = t.blockY + t.shape[t.orientation][2 * k + 1];
                c[tmpX][tmpY]++;
            }
    }
    return ans;
}
const int pnum = 6;
double p[pnum];
double Eval(Board a, const Block &bl)
{
    a.place(bl);
    int rowelim = a.eliminate();

    //double land = block.x - rowelim - blockHalfHeight[block.t][block.o]
    //              + (blockHeight[block.t][block.o] - 1) / 2.0 - 1;

    int cntdown[MAPHEIGHT + 2][MAPWIDTH + 2] = {};
    for (int i = 1; i <= MAPHEIGHT; ++i)
        for (int j = 1; j <= MAPWIDTH; ++j)
            if (!a[i][j])
                cntdown[i][j] = 1 + cntdown[i - 1][j];

    int rowtrans = 0;
    for (int i = 1; i <= MAPHEIGHT; ++i)
        for (int j = 1; j <= MAPWIDTH + 1; ++j)
            if (!!a[i][j] != !!a[i][j - 1])
                ++rowtrans;

    int holenum = 0;
    int row[MAPWIDTH + 2] = {};
    for (int i = MAPHEIGHT - 1; i > 0; --i) {
        for (int j = 1; j <= MAPWIDTH; ++j)
            row[j] = (!a[i][j]) & (!!a[i + 1][j] | row[j]);
        for (int j = 1; j <= MAPWIDTH; ++j)
            holenum += row[j];
    }

    int coltrans = 0;
    for (int i = 1; i <= MAPHEIGHT; ++i)
        for (int j = 1; j <= MAPWIDTH; ++j)
            if (!!a[i][j] != !!a[i - 1][j])
                ++coltrans;

    int wellsum = 0;
    for (int i = 1; i <= MAPHEIGHT; ++i)
        for (int j = 1; j <= MAPWIDTH; ++j)
            if (!a[i][j] && a[i][j - 1] && a[i][j + 1])
                wellsum += cntdown[i][j];


    int maxheight = 0;
    for (int i = MAPHEIGHT; i >= 1; --i) {
        int cnt = 0;
        for (int j = 1; j <= MAPWIDTH; ++j)
            if (a[i][j])
                ++cnt;
        if (cnt) {
            maxheight = i;
            break;
        }
    }

    return  -p[0] * maxheight
            +p[1] * rowelim
            -p[2] * rowtrans
            -p[3] * coltrans
            -p[4] * holenum
            -p[5] * wellsum;
}

double myplace(int this_bl_type, int &finalX, int &finalY, int &finalO)
{
    std::vector<Tetris> loc;
    gb.getPlaces(0, this_bl_type, loc);
    int i;
    double best_val = -1e9, now_val;
    Block now_bl, best_bl = Block(-1, -1, -1, -1);
    Board myBoard(0, gb);
    for (i = 0; i < loc.size(); i++) {
        now_bl = Block(loc[i]);
        now_val = Eval(myBoard, now_bl);
        if (now_val > best_val) {
            best_val = now_val;
            best_bl = now_bl;
        }
    }
    finalX = best_bl.x;
    finalY = best_bl.y;
    finalO = best_bl.o;
    return best_val;
}

int main()
{
    for (int i = 0; i < pnum; ++i)
        scanf("%lf", p + i);
    gb.currBotColor = 0;
    gb.enemyColor = 1;
    srand(getpid() * time(0));
    //srand(0);
    turn = 1;
    int block = 4;
    //int block = rand() % 7;
    while (gb.canPut(0, block)) {
        //printInfo(gb);
        ++gb.typeCountForColor[0][block];
        int fx, fy, fo;
        myplace(block, fx, fy, fo);
        //naive_place(gb, 0, block, evaluate2, fx, fy, fo);

        memcpy(gb.gridInfo[1], gb.gridInfo[0], sizeof(gb.gridInfo[0]));
        memcpy(gb.typeCountForColor[1], gb.typeCountForColor[0], sizeof(gb.typeCountForColor[0]));
        gb.enemyType = block;
        int b2;
        naive_jam(gb, evaluate2, b2);

        gb.place(0, block, fx, fy, fo);
        gb.eliminate(0);
        block = b2;
        ++turn;
        int maxheight = 0;
        for (int i = MAPHEIGHT; i >= 1; --i) {
            int cnt = 0;
            for (int j = 1; j <= MAPWIDTH; ++j)
                if (gb.gridInfo[0][i][j])
                    ++cnt;
            if (cnt) {
                maxheight = i;
                break;
            }
        }
        if (maxheight == MAPHEIGHT)
            break;
        if (turn % 3 == 0) {
            for (int i = maxheight + 1; i > 1; --i)
                memcpy(gb.gridInfo[0][i], gb.gridInfo[0][i - 1], sizeof(gb.gridInfo[0][0]));
            for (int i = 1; i <= MAPWIDTH; ++i)
                gb.gridInfo[0][1][i] = 1;
            gb.gridInfo[0][1][rand() % MAPWIDTH + 1] = 0;
            if (rand() % 2)
                gb.gridInfo[0][1][rand() % MAPWIDTH + 1] = 0;
        }
    }
    printf("%d\n", turn);
    return 0;
}
