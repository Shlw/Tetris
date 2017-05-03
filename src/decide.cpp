#include "decide.h"
#include <algorithm>

void random_place(GameBoard &gameBoard, int this_col, int this_bl_type, std::function<double(Board, const Block&)> Eval, int &finalX, int &finalY, int &finalO)
{
    std::vector<Tetris> loc;
    gameBoard.getPlaces(this_col, this_bl_type, loc);
    Tetris &ans = loc[rand() % loc.size()];
    finalX = ans.blockX;
    finalY = ans.blockY;
    finalO = ans.orientation;
}

double naive_place(GameBoard &gameBoard, int this_col, int this_bl_type, std::function<double(Board, const Block &)> Eval, int &finalX, int &finalY, int &finalO)
{
    std::vector<Tetris> loc;
    gameBoard.getPlaces(this_col, this_bl_type, loc);
    int i;
    double best_val = -1e9, better_val = -1e9, now_val;
    Block now_bl, best_bl = Block(-1, -1, -1, -1),
                  better_bl = Block(-1, -1, -1, -1);
    Board myBoard(this_col, gameBoard);
    for (i = 0; i < loc.size(); i++) {
        now_bl = Block(loc[i]);
        now_val = Eval(myBoard, now_bl);
        if (now_val > best_val) {
            better_val = best_val;
            better_bl = best_bl;
            best_val = now_val;
            best_bl = now_bl;
        }
        else if (now_val > better_val) {
            better_val = now_val;
            better_bl = now_bl;
        }
    }
    double ret;
    if (better_bl.x == -1 || rand() % 10 < 8) {
        finalX = best_bl.x;
        finalY = best_bl.y;
        finalO = best_bl.o;
        ret = best_val;
    }
    else {
        finalX = better_bl.x;
        finalY = better_bl.y;
        finalO = better_bl.o;
        ret = better_val;
    }
    return ret;
}

void random_jam(GameBoard &gameBoard, std::function<double(Board, const Block &)> Eval, int &blockForEnemy)
{
    int min_type = 1e9;
    for (int i = 0; i < 7; i++)
        min_type = std::min(gameBoard.typeCountForColor[gameBoard.enemyColor][i], min_type);
    int lst[7], nlst = 0;
    for (int i = 0; i < 7; i++)
        if (gameBoard.typeCountForColor[gameBoard.enemyColor][i] <= min_type + 1)
            lst[nlst++] = i;
    blockForEnemy = lst[rand() % nlst];
}

void naive_jam(GameBoard &gameBoard, std::function<double(Board, const Block &)> Eval, int &blockForEnemy)
{
    int f_x, f_y, f_o, min_type = 1e9, now_col;
    naive_place(gameBoard, gameBoard.enemyColor, gameBoard.enemyType, Eval, f_x, f_y, f_o);
    gameBoard.place(gameBoard.enemyColor, gameBoard.enemyType, f_x, f_y, f_o);
    gameBoard.eliminate(gameBoard.enemyColor);
    for (int i = 0; i < 7; i++)
        min_type = std::min(gameBoard.typeCountForColor[gameBoard.enemyColor][i], min_type);
    double now_val, worst_val = 1e9, worse_val = 1e9;
    int worst_bl = -1, worse_bl = -1;
    for (int i = 0; i < 7; i++) {
        if (gameBoard.typeCountForColor[gameBoard.enemyColor][i] <= min_type + 1) {
            now_val = naive_place(gameBoard, gameBoard.enemyColor, i, Eval, f_x, f_y, f_o);
            if (now_val < worst_val) {
                worse_val = worst_val;
                worse_bl = worst_bl;
                worst_val = now_val;
                worst_bl = i;
            }
            else if (now_val < worse_val) {
                worse_val = now_val;
                worse_bl = i;
            }
        }
    }
    if (worse_bl == -1 || rand() % 10 < 6)
        blockForEnemy = worst_bl;
    else
        blockForEnemy = worse_bl;
}
