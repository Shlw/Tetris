#include "game.h"
#include "util.h"

#include <cmath>
#include <algorithm>
#include <utility>
#include <vector>
#include <string>
#include <cstring>

#define RED_HOLE_HEIGHT 14
#define RED_HOLE_WEIGHT 0.15
using namespace std;
extern int bitcount[1 << 12];

void eval(Board& brd, const Block& block, int& ele, int& ht) {
    int basenum = brd.place(block);
    pair<int, int> elim=brd.eliminate(&block);
    ele = elim.first;
    for (int i = MAPHEIGHT; i > 0; --i)
        if (brd.rows[i]!=Board::EMPTY_ROW) {
            ht = i;
            break;
        }
}

void precalc(GameBoard& gameBoard, int& myele, int& enemyele, int& enemyht) {
    std::vector<Tetris> myloc;
    gameBoard.getPlaces(gameBoard.currBotColor, gameBoard.currBlockType, myloc);
    std::vector<Tetris> enemyloc;
    gameBoard.getPlaces(gameBoard.enemyColor, gameBoard.enemyType, enemyloc);
    myele = 0;
    enemyele = 0;
    enemyht = 20;
    for (auto& loc : myloc) {
        int ht, ele;
        Board brd = Board(gameBoard.currBotColor, gameBoard);
        eval(brd, Block(loc), ele, ht);
        myele = std::max(myele, ele);
    }
    for (auto& loc : enemyloc) {
        int ht, ele;
        Board brd = Board(gameBoard.enemyColor, gameBoard);
        eval(brd, Block(loc), ele, ht);
        enemyele = std::max(enemyele, ele);
        enemyht = std::min(enemyht, ht);
    }
}

double evaluate2_sweet(Board brd, const Block &block, double &inh, bool last_layer)
{
    int basenum = brd.place(block);
    pair<int, int> elim = brd.eliminate(&block);
    int *rows = brd.rows;
    int *cols = brd.cols;

    double sweet = 0;
    if (basenum == 4) {
        //sweet += 10;
        sweet += 20;
        if (elim.second == 3)
            sweet += 50;
        if (elim.second == 4)
            sweet += 100;
    }

    double land = block.x - elim.first - blockHalfHeight[block.t][block.o]
                  + (blockHeight[block.t][block.o] - 1) / 2.0 - 1;

    //int cntdown[MAPWIDTH + 2] = {};
    int rowtrans = 0;
    int wellsum = 0;
    for (int i = 1; i <= MAPHEIGHT; ++i) {
        int cur = rows[i];
        rowtrans += bitcount[cur ^ (cur >> 1)] - 1;
        cur = (~cur) & (cur >> 1) & (cur << 1);
        int j = i;
        while (cur) {
            wellsum += bitcount[cur];
            cur &= ~rows[--j];
        }
    }

    int coltrans = 0;
    for (int i = 1; i <= MAPWIDTH; ++i) {
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


    int maxheight = 0;
    for (int i = MAPHEIGHT; i >= 1; --i)
        if (rows[i]^Board::EMPTY_ROW) {
            maxheight = i;
            break;
        }

    const double p[6] = {5.00016, 1.11813, 6.71788, 12.3487, 11.3993, 8.2856};
    inh = - p[0] * land
          + p[1] * elim.first
          + sweet;

    double red_hole = 0;
    if (last_layer == 1)
        red_hole = max(0, RED_HOLE_HEIGHT - maxheight) * RED_HOLE_WEIGHT;
    return  - p[0] * land
            + p[1] * elim.first
            - p[2] * rowtrans
            - (p[3] + red_hole) * coltrans
            - (p[4] + red_hole) * holenum
            - p[5] * wellsum
            + sweet;
    /*inh = - 4.500158825082766 * land
          + 3.4181268101392694 * elim.first
          + sweet;

    return  - 4.500158825082766 * land
            + 3.4181268101392694 * elim.first
            - 3.2178882868487753 * rowtrans
            - 9.348695305445199 * coltrans
            - 7.899265427351652 * holenum
            - 3.3855972247263626 * wellsum
            + sweet;*/
}
