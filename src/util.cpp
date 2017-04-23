#include "util.h"
#include <cstring>

Block::Block(int _x, int _y, int _o, int _t)
    : x(_x), y(_y), o(_o), t(_t) {}

Board::Board()
{
    for (int i = 0; i < MAPHEIGHT + 2; i++)
        grid[i][0] = grid[i][MAPWIDTH + 1] = -2;
    for (int i = 0; i < MAPWIDTH + 2; i++)
        grid[0][i] = grid[MAPHEIGHT + 1][i] = -2;
}

void Board::place(const Block &block)
{
    int i, tmpX, tmpY;
    auto shape = blockShape[block.t][block.o];
    for (i = 0; i < 4; i++) {
        tmpX = block.x + shape[2 * i];
        tmpY = block.y + shape[2 * i + 1];
        grid[tmpX][tmpY] = 1;
    }
}

void Board::deplace(const Block &block)
{
    int i, tmpX, tmpY;
    auto shape = blockShape[block.t][block.o];
    for (i = 0; i < 4; i++) {
        tmpX = block.x + shape[2 * i];
        tmpY = block.y + shape[2 * i + 1];
        grid[tmpX][tmpY] = 0;
    }
}

int Board::eliminate()
{
    int count = 0;
    for (int i = 1; i <= MAPHEIGHT; ++i) {
        bool full = true, empty = true;
        for (int j = 1; j <= MAPWIDTH; ++j)
            if (grid[i][j] != 0)
                empty = false;
            else
                full = false;
        if (empty)
            break;
        if (full)
            ++count;
    }
    if (count == 0)
        return 0;
    for (int i = 1; i <= MAPHEIGHT - count; ++i)
        memcpy(grid[i] + 1, grid[i + count] + 1, sizeof(int) * MAPWIDTH);
    for (int i = 0; i < count; ++i)
        memset(grid[MAPHEIGHT - i] + 1, 0, sizeof(int) * MAPWIDTH);
    return count;
}
