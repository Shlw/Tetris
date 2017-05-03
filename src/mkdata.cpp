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
    auto a = gb.gridInfo[0];
    /*for (int i = 1; i <= MAPWIDTH; ++i) {
        int mh = MAPHEIGHT;
        while (a[mh][i] == 0)
            --mh;
        int hole = mh - 1;
        while (hole >= 0 && a[hole][i] != 0)
            --hole;
        int hd;
        if (hole == -1)
            hd = -1;
        else {
            hd = 1;
            while (a[hole - hd][i] == 0)
                ++hd;
        }
        printf("%g ", mh / 10.0);
    }*/

    int rowtrans=0;
    for (int i=1;i<=MAPHEIGHT;++i)
        for (int j=1;j<=MAPWIDTH+1;++j)     
            if (!!a[i][j] != !!a[i][j-1])
                ++rowtrans;

    int holenum=0;
    int row[MAPWIDTH+2]={};
    for (int i=MAPHEIGHT-1;i>0;--i){
        for (int j=1;j<=MAPWIDTH;++j) 
            row[j]=(!a[i][j])&(!!a[i+1][j] | row[j]);
        for (int j=1;j<=MAPWIDTH;++j)
            holenum+=row[j];
    }

    int coltrans=0;
    for (int i=1;i<=MAPHEIGHT;++i)
        for (int j=1;j<=MAPWIDTH;++j)
            if (!!a[i][j] != !!a[i-1][j])
                ++coltrans;

    int wellsum=0;
    for (int i=1;i<=MAPHEIGHT;++i)
        for (int j=1;j<=MAPWIDTH;++j)
            if (!a[i][j] && a[i][j-1] && a[i][j+1])
                wellsum++;
                //wellsum+=cntdown[i][j];
                
    int maxheight=0;
    for (int i=MAPHEIGHT;i>=1;--i){
        int cnt=0;
        for (int j=1;j<=MAPWIDTH;++j)
            if (a[i][j])
                ++cnt;
        if (cnt){
            maxheight=i;
            break;
        }
    }

    printf("%g %g %g %g %g ", rowtrans/40.0, holenum/15.0, coltrans/40.0, wellsum/20.0, maxheight/15.0);
}

void run(int block)
{
    printStatus();
    GameBoard gb2 = gb;
    int turn_left = 0;
    while (gb2.canPut(0, block)) {
        ++gb2.typeCountForColor[0][block];
        int fx, fy, fo;
        naive_place(gb2, 0, block, evaluate2, fx, fy, fo);

        memcpy(gb2.gridInfo[1], gb2.gridInfo[0], sizeof(gb2.gridInfo[0]));
        memcpy(gb2.typeCountForColor[1], gb2.typeCountForColor[0], sizeof(gb2.typeCountForColor[0]));
        gb2.enemyType = block;
        int b2;
        naive_jam(gb2, evaluate2, b2);

        gb2.place(0, block, fx, fy, fo);
        gb2.eliminate(0);
        block = b2;
        ++turn_left;
    }
    printf("%d\n", turn_left);
}

int main()
{
    gb.currBotColor = 0;
    gb.enemyColor = 1;
    srand(getpid() * time(0));
    //srand(0);
    turn = 1;
    int block = rand() % 7;
    while (gb.canPut(0, block)) {
        //printStatus();
        //printInfo();

        ++gb.typeCountForColor[0][block];
        int fx, fy, fo;
        if (rand() % 10 < 6)
            naive_place(gb, 0, block, evaluate2, fx, fy, fo);
        else
            random_place(gb, 0, block, evaluate2, fx, fy, fo);

        memcpy(gb.gridInfo[1], gb.gridInfo[0], sizeof(gb.gridInfo[0]));
        memcpy(gb.typeCountForColor[1], gb.typeCountForColor[0], sizeof(gb.typeCountForColor[0]));
        gb.enemyType = block;
        int b2;
        random_jam(gb, evaluate2, b2);

        gb.place(0, block, fx, fy, fo);
        gb.eliminate(0);
        block = b2;
        ++turn;

        if (turn >= 3)
            run(block);
    }
    //printf("%d\n", turn);
    return 0;
}
