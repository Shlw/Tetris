// 注意：x的范围是1~MAPWIDTH，y的范围是1~MAPHEIGHT
// 数组是先行（y）后列（c）
// 坐标系：原点在左下角

#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <functional>
#include "jsoncpp/json.h"
#include "game.h"
#include "decide.h"
using namespace std;

GameBoard gameBoard;

void init(const Json::Value &input)
{
    //srand(time(NULL));
    srand(0);

    auto &first = input["requests"][(Json::UInt)0];
    gameBoard.play(input);
}

void decide(int &blockForEnemy, int &finalX, int &finalY, int &finalO)
{
    naive_place(gameBoard, gameBoard.currBotColor, gameBoard.currBlockType, evaluate1, finalX, finalY, finalO);
    naive_jam(gameBoard, evaluate1, blockForEnemy);
}

void readInput(Json::Value &input)
{
    // 读入JSON
    string str, line;
    while (getline(cin, line))
        str += line;
    Json::Reader reader;
    reader.parse(str, input);
}
int main()
{
    Json::Value input;
    readInput(input);

    init(input);

    int blockForEnemy, finalX, finalY, finalO;
    decide(blockForEnemy, finalX, finalY, finalO);

    Json::Value output;
    Json::FastWriter writer;

    output["response"]["block"] = blockForEnemy;

    output["response"]["x"] = finalY;
    output["response"]["y"] = finalX;
    output["response"]["o"] = finalO;

    cout << writer.write(output);

    return 0;
}
