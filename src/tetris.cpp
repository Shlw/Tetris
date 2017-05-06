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
#include <cstring>

using namespace std;
#define debug(x) cerr<<#x<<"="<<x<<endl

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
    set_Eval(evaluate2_sweet);
    int tmp;
    Place_Turn(1, gameBoard, gameBoard.currBotColor, gameBoard.currBlockType, finalX, finalY, finalO, tmp);
    //while(1);
    Place_Turn(1, gameBoard, gameBoard.currBotColor ^ 1, gameBoard.enemyType, tmp, tmp, tmp, blockForEnemy);
    //naive_place(gameBoard, gameBoard.currBotColor, gameBoard.currBlockType, evaluate3, finalX, finalY, finalO);
    //naive_jam(gameBoard, evaluate3, blockForEnemy);
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

int bitcount[1 << 12];

void prepare(){
    memset(bitcount, 0, sizeof(bitcount));
    for (int i = 0; i < (1 << 12); ++i)
        for (int j = 0; j < 12; ++j)
            if (i & (1 << j))  ++bitcount[i];
}

int main()
{
    //freopen("debug_in.txt","r",stdin);
    prepare();
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
