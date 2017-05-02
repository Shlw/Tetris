#ifndef DECIDE_H
#define DECIDE_H

#include "game.h"
#include "util.h"
#include <functional>

double naive_place(GameBoard &gameBoard, int this_col, int this_bl_type, std::function<double(Board, const Block&)> Eval, int &finalX, int &finalY, int &finalO);
void naive_jam(GameBoard &gameBoard, std::function<double(Board, const Block&)> Eval, int &blockForEnemy);
void random_jam(GameBoard &gameBoard, std::function<double(Board, const Block&)> Eval, int &blockForEnemy);

#endif
