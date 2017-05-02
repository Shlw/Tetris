#include "jsoncpp/json.h"
#include "game.h"
#include "decide.h"

#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <functional>
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
    naive_place(gameBoard, gameBoard.currBotColor, gameBoard.currBlockType, evaluate2, finalX, finalY, finalO);
    //random_jam(gameBoard, evaluate2, blockForEnemy);
    naive_jam(gameBoard, evaluate2, blockForEnemy);
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
