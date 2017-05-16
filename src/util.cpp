#include "util.h"
#include <cstring>
#include <algorithm>

Block::Block(int _x, int _y, int _o, int _t)
    : x(_x), y(_y), o(_o), t(_t) {}

Block::Block(Tetris te)
{
    x = te.blockX;
    y = te.blockY;
    o = te.orientation;
    t = te.blockType;
}

void Board::setFrame()
{
    rows[MAPHEIGHT + 1] = 0;
    rows[0] = FULL_ROW;
    cols[0] = cols[MAPWIDTH + 1] = FULL_COL;
    for (int i = 1; i <= MAPHEIGHT; ++i)
        rows[i] = EMPTY_ROW;
    for (int i = 1; i <= MAPWIDTH; ++i)
        cols[i] = EMPTY_COL;
}

Board::Board() {setFrame();}

Board::Board(int color, const GameBoard &gameBoard)
{
    setFrame();
    for (int i = 1; i <= MAPHEIGHT; ++i)
        for (int j = 1; j <= MAPWIDTH; ++j)
            if (gameBoard.gridInfo[color][i][j]) {
                rows[i] |= 1 << j;
                cols[j] |= 1 << i;
            }
}

void Board::place(const Block &block)
{
    int i, tmpX, tmpY;
    auto shape = blockShape[block.t][block.o];
    for (i = 0; i < 4; ++i) {
        tmpX = block.x + shape[2 * i];
        tmpY = block.y + shape[2 * i + 1];
        rows[tmpX] |= 1 << tmpY;
        cols[tmpY] |= 1 << tmpX;
    }
}

void Board::deplace(const Block &block)
{
    int i, tmpX, tmpY;
    auto shape = blockShape[block.t][block.o];
    for (i = 0; i < 4; ++i) {
        tmpX = block.x + shape[2 * i];
        tmpY = block.y + shape[2 * i + 1];
        rows[tmpX] ^= 1 << tmpY;
        cols[tmpY] ^= 1 << tmpX;
    }
}

void Board::eliminate(const Block *block, std::pair<int, int> *elim, int *nobase)
{
    int count = 0;
    for (int i = 1; i <= MAPHEIGHT; ++i)
        if (rows[i] == EMPTY_ROW)
            break;
        else if (rows[i] == FULL_ROW)
            ++count;

    if (block) {
        elim->first = count;
        int count2 = 0;
        auto shape = blockShape[block->t][block->o];
        for (int i = 0; i < 4; ++i)
            if (rows[block->x + shape[2 * i]] == FULL_ROW)
                ++count2;
        elim->second = count2;
        *nobase = 0;
        for (int i = 0; i < 4; ++i) {
            int tmpX = block->x + shape[2 * i];
            int tmpY = block->y + shape[2 * i + 1];
            if (rows[tmpX] != FULL_ROW) {
                int x = tmpX - 1;
                while (x && rows[x] == FULL_ROW)
                    --x;
                if ((rows[x] & (1 << tmpY)) == 0)
                    ++*nobase;
            }
        }
    }
    for (int i = MAPHEIGHT; i > 0; --i)
        if (rows[i] == FULL_ROW) {
            for (int j = i; j < MAPHEIGHT; ++j)
                rows[j] = rows[j + 1];
            rows[MAPHEIGHT] = EMPTY_ROW;
            for (int j = 1; j <= MAPWIDTH; ++j)
                cols[j] = (cols[j] & ((1 << (i + 1)) - 1)) |
                          ((cols[j] >> (i + 1)) << i);
        }
}

