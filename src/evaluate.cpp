#include "game.h"
#include "util.h"

#include <cmath>
#include <algorithm>
#include <utility>
#include <vector>
#include <string>
#include <cstring>

using namespace std;
extern int bitcount[1 << 12];

/*
double evaluate2(Board a, const Block &block, double &inh)
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

    
    bool visible[MAPHEIGHT+2][MAPWIDTH+2]={};
    for (int i=1;i<=MAPWIDTH;++i) visible[MAPHEIGHT+1][i]=1;
    for (int i=MAPHEIGHT;i>0;--i)
        for (int j=1;j<=MAPWIDTH;++j)
            visible[i][j]=visible[i+1][j]&&(!a[i][j]);
    

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

    inh = - 4.500158825082766 * land
            + 3.4181268101392694 * rowelim;
    return  - 4.500158825082766 * land
            + 3.4181268101392694 * rowelim
            - 3.2178882868487753 * rowtrans
            - 9.348695305445199 * coltrans
            - 7.899265427351652 * holenum
            - 3.3855972247263626 * wellsum;
}
*/
double evaluate2_sweet(Board brd, const Block &block, double &inh)
{
    brd.place(block);
    pair<int, int> elim = brd.eliminate(&block);
    int* rows = brd.rows;
    int* cols = brd.cols;

    double sweet = 0;
    if (elim.second == 3)
        sweet += 50;
    if (elim.second == 4)
        sweet += 100;

    double land = block.x - elim.first - blockHalfHeight[block.t][block.o]
                  + (blockHeight[block.t][block.o] - 1) / 2.0 - 1;

    int cntdown[MAPWIDTH + 2] = {};
    int rowtrans = 0;
    int wellsum = 0;
    for (int i = 1; i <= MAPHEIGHT; ++i){
        int cur = rows[i];
        rowtrans += bitcount[cur ^ (cur >> 1)] - 1;
        cur = (~cur) & (cur >> 1) & (cur << 1);
        int j = i;
        while (cur){
            wellsum += bitcount[cur];
            cur &= ~rows[--j];
        }
    }

    int coltrans = 0;
    for (int i = 1; i <= MAPWIDTH; ++i){
        int cur = cols[i];
        cur ^= cur >> 1;
        coltrans += bitcount[cur & 2047] + bitcount[cur >> 12];
    }

    int holenum = 0;
    int row = 0;
    for (int i = MAPHEIGHT - 1; i > 0; --i) {
        row = (~rows[i]) & (rows[i + 1] | row);
        holenum += bitcount[row & 2047] + bitcount[row >> 12];
    }

/*
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
*/

    inh = - 4.500158825082766 * land
          + 3.4181268101392694 * elim.first
          + sweet;

    return  - 4.500158825082766 * land
            + 3.4181268101392694 * elim.first
            - 3.2178882868487753 * rowtrans
            - 9.348695305445199 * coltrans
            - 7.899265427351652 * holenum
            - 3.3855972247263626 * wellsum
            + sweet;
}
