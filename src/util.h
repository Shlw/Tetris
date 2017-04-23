#ifndef UTIL_H
#define UTIL_H

#include "game.h"

class Block
{
public:
    int x, y, o;
    int t; // 类型

    Block() {}
    Block(int _x, int _y, int _o, int _t);
    Block(Tetris te);
};

class Board
{
public:
    int grid[MAPHEIGHT + 2][MAPWIDTH + 2];

    Board();
    Board(int color, const GameBoard &gameBoard);
    void place(const Block &);
    void deplace(const Block &);
    int eliminate(); // 返回消去的行数

    int *operator[] (int index);
};

double evaluate1(Board a,const Block& block);
double evaluate2(Board a,const Block& block);

#endif 
