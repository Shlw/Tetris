#ifndef UTIL_H
#define UTIL_H

#include "game.h"
#include <utility>

class Block {
    public:
        int x, y, o;
        int t; // 类型

        Block() {}
        Block(int _x, int _y, int _o, int _t);
        Block(Tetris te);
};

class Board {
    public:
        const static int FULL_ROW = (1 << (MAPWIDTH + 2)) - 1;
        const static int FULL_COL = (1 << (MAPHEIGHT + 1)) - 1;
        const static int EMPTY_ROW = 1 | (1 << (MAPWIDTH + 1));
        const static int EMPTY_COL = 1;
        int rows[MAPHEIGHT + 2];
        int cols[MAPWIDTH + 2];

        Board();
        Board(int color, const GameBoard& gameBoard);
        void setFrame();
        void place(const Block&);
        void deplace(const Block&);
        void eliminate(const Block* block = NULL, std::pair<int, int>* elim = NULL, int* nobase = NULL);

};

void precalc(GameBoard& gameBoard, int& myele, int& enemyele, int& enemyht);
double evaluate2_sweet(Board a, const Block& block, double&);

extern int bitcount[1 << 12];

#endif
