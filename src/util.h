#ifndef UTIL_H
#define UTIL_H

#include "game.h"

class Block
{
public:
    int x, y, o;
    int t; // 类型

    Block(int _x, int _y, int _o, int _t);
};

class Board
{
public:
    int grid[MAPHEIGHT + 2][MAPWIDTH + 2];

    Board();
    void place(const Block &);
    void deplace(const Block &);
    int eliminate(); // 返回消去的行数
};

#endif 
