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

Board::Board()
{
    for (int i = 0; i < MAPHEIGHT + 2; i++)
        grid[i][0] = grid[i][MAPWIDTH + 1] = -2;
    for (int i = 0; i < MAPWIDTH + 2; i++)
        grid[0][i] = grid[MAPHEIGHT + 1][i] = -2;
}

Board::Board(int color, const GameBoard &gameBoard)
{
    for (int i = 0; i < MAPHEIGHT + 2; i++)
        grid[i][0] = grid[i][MAPWIDTH + 1] = -2;
    for (int i = 0; i < MAPWIDTH + 2; i++)
        grid[0][i] = grid[MAPHEIGHT + 1][i] = -2;
    for (int i = 1; i <= MAPHEIGHT; i++)
        for (int j = 1; j <= MAPWIDTH; j++)
            grid[i][j] = gameBoard.gridInfo[color][i][j];
}

int Board::place(const Block &block)
{
    int i, tmpX, tmpY;
    auto shape = blockShape[block.t][block.o];
    for (i = 0; i < 4; i++) {
        tmpX = block.x + shape[2 * i];
        tmpY = block.y + shape[2 * i + 1];
        grid[tmpX][tmpY] = 1;
    }
    int basenum = 0;
    for (i = 0; i < 4; i++) {
        tmpX = block.x + shape[2 * i];
        tmpY = block.y + shape[2 * i + 1];
        if (grid[tmpX - 1][tmpY] != 0)
            basenum++;
    }
    return basenum;
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

std::pair<int, int> Board::eliminate(const Block *block)
{
    int count = 0;
    bool linefull[MAPHEIGHT + 2] = {};
    for (int i = 1; i <= MAPHEIGHT; ++i) {
        bool full = true, empty = true;
        for (int j = 1; j <= MAPWIDTH; ++j)
            if (grid[i][j] != 0)
                empty = false;
            else
                full = false;
        if (empty)
            break;
        if (full) {
            ++count;
            linefull[i] = true;
        }
    }
    if (count == 0)
        return std::make_pair(0, 0);
    int count2 = 0;
    if (block) {
        auto shape = blockShape[block->t][block->o];
        for (int i = 0; i < 4; i++)
            if (linefull[block->x + shape[2 * i]])
                ++count2;
    }
    for (int i = 1; i <= MAPHEIGHT - count; ++i)
        memcpy(grid[i] + 1, grid[i + count] + 1, sizeof(int) * MAPWIDTH);
    for (int i = 0; i < count; ++i)
        memset(grid[MAPHEIGHT - i] + 1, 0, sizeof(int) * MAPWIDTH);
    return std::make_pair(count, count2);
}

int *Board::operator[](int index)
{
    return grid[index];
}
