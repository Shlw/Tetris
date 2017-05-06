#include "game.h"
#include "util.h"

#include <cmath>
#include <algorithm>
#include <utility>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

double evaluate2(Board a, const Block &block)
{
    a.place(block);
    int rowelim = a.eliminate().first;

    double land = block.x - rowelim - blockHalfHeight[block.t][block.o]
                  + (blockHeight[block.t][block.o] - 1) / 2.0 - 1;

    int cntdown[MAPHEIGHT + 2][MAPWIDTH + 2] = {};
    for (int i = 1; i <= MAPHEIGHT; ++i)
        for (int j = 1; j <= MAPWIDTH; ++j)
            if (!a[i][j])
                cntdown[i][j] = 1 + cntdown[i - 1][j];

    /*
    bool visible[MAPHEIGHT+2][MAPWIDTH+2]={};
    for (int i=1;i<=MAPWIDTH;++i) visible[MAPHEIGHT+1][i]=1;
    for (int i=MAPHEIGHT;i>0;--i)
        for (int j=1;j<=MAPWIDTH;++j)
            visible[i][j]=visible[i+1][j]&&(!a[i][j]);
    */

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

    return  - 4.500158825082766 * land
            + 3.4181268101392694 * rowelim
            - 3.2178882868487753 * rowtrans
            - 9.348695305445199 * coltrans
            - 7.899265427351652 * holenum
            - 3.3855972247263626 * wellsum;
}

double evaluate2_sweet(Board a, const Block &block)
{
    if (block.x == 7 && block.y == 2 && block.o == 1 || block.x == 9 && block.y == 4 && block.o == 1)
        printf("!!\n");
    int basenum = a.place(block);
    pair<int, int> elim = a.eliminate(&block);

    double sweet = 0;
    if (basenum == 4)
        sweet += 20;
    if (elim.second == 3)
        sweet += 50;
    if (elim.second == 4)
        sweet += 100;

    double land = block.x - elim.first - blockHalfHeight[block.t][block.o]
                  + (blockHeight[block.t][block.o] - 1) / 2.0 - 1;

    int cntdown[MAPHEIGHT + 2][MAPWIDTH + 2] = {};
    for (int i = 1; i <= MAPHEIGHT; ++i)
        for (int j = 1; j <= MAPWIDTH; ++j)
            if (!a[i][j])
                cntdown[i][j] = 1 + cntdown[i - 1][j];

    /*
    bool visible[MAPHEIGHT+2][MAPWIDTH+2]={};
    for (int i=1;i<=MAPWIDTH;++i) visible[MAPHEIGHT+1][i]=1;
    for (int i=MAPHEIGHT;i>0;--i)
        for (int j=1;j<=MAPWIDTH;++j)
            visible[i][j]=visible[i+1][j]&&(!a[i][j]);
    */

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

    return  - 4.500158825082766 * land
            + 3.4181268101392694 * elim.first
            - 3.2178882868487753 * rowtrans
            - 9.348695305445199 * coltrans
            - 7.899265427351652 * holenum
            - 3.3855972247263626 * wellsum
            + sweet;
}
