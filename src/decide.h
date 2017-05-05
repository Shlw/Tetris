#ifndef DECIDE_H
#define DECIDE_H

#include "game.h"
#include "util.h"
#include <vector>
#include <functional>

void set_Eval(std::function<double(Board, const Block&)> EFun);
double naive_place(GameBoard &gameBoard, int this_col, int this_bl_type);
double Place_Turn(int dep, GameBoard& gameBoard, int pl_col, int this_bl_type, int &finalX, int &finalY, int &finalO, int &blockFE); //blockFE表示给敌人的块
std::vector<int> Jam_Turn(int dep, GameBoard &gameBoard, int pl_col, std::vector<double> &A_i); //返回可行方案编号
double naive_place2(GameBoard &gameBoard, int this_col, int this_bl_type, std::function<double(Board, const Block&)> Eval, int &finalX, int &finalY, int &finalO);
//double naive_place(GameBoard &gameBoard, int this_col, int this_bl_type, std::function<double(Board, const Block&)> Eval, int &finalX, int &finalY, int &finalO);
//void naive_jam(GameBoard &gameBoard, std::function<double(Board, const Block&)> Eval, int &blockForEnemy);

#endif
