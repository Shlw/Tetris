#ifndef UTIL_H
#define UTIL_H

#include "game.h"
#include <utility>

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
    int place(const Block &);
    void deplace(const Block &);
    std::pair<int,int> eliminate(const Block *block = NULL); // 返回消去的行数，块数

    int *operator[] (int index);
};

double evaluate1(Board a,const Block& block);
double evaluate2(Board a,const Block& block);
double evaluate2_sweet(Board a,const Block& block);

#endif 
