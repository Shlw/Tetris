// 注意：x的范围是1~MAPWIDTH，y的范围是1~MAPHEIGHT
// 数组是先行（y）后列（c）
// 坐标系：原点在左下角

#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "jsoncpp/json.h"
#include "game.h"
using namespace std;

// 我所在队伍的颜色（0为红，1为蓝，仅表示队伍，不分先后）
int currBotColor;
int enemyColor;
int currBlockType;

GameBoard gameBoard;

void init(const Json::Value &input)
{
    srand(time(NULL));

    auto &first = input["requests"][(Json::UInt) 0];
    currBlockType = first["block"].asInt();
    currBotColor = first["color"].asInt();
    enemyColor = 1 - currBotColor;

    currBlockType = gameBoard.play(input);
}

void decide(int &blockForEnemy, int &finalX, int &finalY, int &finalO)
{
    // 做出决策（你只需修改以下部分）

    // 贪心决策
    // 从下往上以各种姿态找到第一个位置，要求能够直着落下
    vector<Tetris> blocks;
    gameBoard.getPlaces(currBotColor, currBlockType, blocks);
    for (auto block : blocks) {
        finalX = block.blockX;
        finalY = block.blockY;
        finalO = block.orientation;
        break;
    }
    /*Tetris block(&gameBoard, currBlockType, currBotColor);
    for (int x = 1; x <= MAPHEIGHT; x++)
        for (int y = 1; y <= MAPWIDTH; y++)
            for (int o = 0; o < 4; o++) {
                if (block.set(x, y, o).isValid() &&
                    gameBoard.checkDirectDropTo(currBotColor, block.blockType, x, y, o)) {
                    finalX = x;
                    finalY = y;
                    finalO = o;
                    goto determined;
                }
            }

determined:*/

    // 再看看给对方什么好

    int maxCount = 0, minCount = 99;
    for (int i = 0; i < 7; i++) {
        if (gameBoard.typeCountForColor[enemyColor][i] > maxCount)
            maxCount = gameBoard.typeCountForColor[enemyColor][i];
        if (gameBoard.typeCountForColor[enemyColor][i] < minCount)
            minCount = gameBoard.typeCountForColor[enemyColor][i];
    }
    if (maxCount - minCount == 2) {
        // 危险，找一个不是最大的块给对方吧
        for (blockForEnemy = 0; blockForEnemy < 7; blockForEnemy++)
            if (gameBoard.typeCountForColor[enemyColor][blockForEnemy] != maxCount)
                break;
    }
    else {
        blockForEnemy = rand() % 7;
    }

    // 决策结束，输出结果（你只需修改以上部分）
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
