#include "decide.h"
#include <algorithm>

double naive_place(GameBoard &gameBoard, int this_col, int this_bl_type, std::function<double(Board, const Block&)> Eval, int &finalX, int &finalY, int &finalO)
{
    std::vector<Tetris> loc;
    gameBoard.getPlaces(this_col, this_bl_type, loc);
    int i;
    double best_val = -1e9, now_val;
    Block now_bl, best_bl;
    Board myBoard(this_col, gameBoard);
    for (i = 0; i < loc.size(); i++)
    {
        now_bl = Block(loc[i]);
        now_val = Eval(myBoard, now_bl);
        if(now_val > best_val)
        {
            best_val = now_val;
            best_bl = now_bl;
        }
    }
    finalX = best_bl.x;
    finalY = best_bl.y;
    finalO = best_bl.o;
    return best_val;
}

void naive_jam(GameBoard &gameBoard, std::function<double(Board, const Block&)> Eval, int &blockForEnemy)
{
    int f_x, f_y, f_o, min_type = 1e9, now_col;
    naive_place(gameBoard, gameBoard.enemyColor, gameBoard.enemyType, Eval, f_x, f_y, f_o);
    gameBoard.place(gameBoard.enemyColor, gameBoard.enemyType, f_x, f_y, f_o);
    gameBoard.eliminate(gameBoard.enemyColor);
    for (int i = 0; i < 7; i++)
        min_type = std::min(gameBoard.typeCountForColor[gameBoard.enemyColor][i], min_type);
    double now_val, worst_val = 1e9;
    int worst_bl;
    for (int i = 0; i < 7; i++)
    {
        if (gameBoard.typeCountForColor[gameBoard.enemyColor][i] <= min_type + 1)
        {
            now_val = naive_place(gameBoard, gameBoard.enemyColor, i, Eval, f_x, f_y, f_o);
            if(now_val < worst_val)
            {
                worst_val = now_val;
                worst_bl = i;
            }
        }
    }
    blockForEnemy = worst_bl;
}
