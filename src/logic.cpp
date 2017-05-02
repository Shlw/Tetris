#include "game.h"
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <string>
#include <fstream>
using namespace std;

int turnID = 1;
GameBoard gameBoard;
Json::Value logger;

void makeInput(int id, string str)
{
    ofstream fin(str);
    Json::FastWriter writer;
    Json::Value input;
    Json::Value &req = input["requests"];
    Json::Value &res = input["responses"];
    for (int i = 1; i <= turnID; ++i)
        req[i - 1] = logger[i - 1][id];
    for (int i = 1; i < turnID; ++i)
        res[i - 1] = logger[i][1 - id];
    fin << writer.write(input);
    fin.close();
}

Json::Value getOutput(string str)
{
    ifstream fin(str);
    Json::Value ans;
    string line, s;
    while (getline(fin, line))
        s += line;
    fin.close();
    Json::Reader reader;
    reader.parse(s, ans);
    return ans["response"];
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        puts("usage: logic player1 player2 output");
        puts("this program will create lots of temperary files !!!");
        return 1;
    }

    int winner = -2;

    while (1) {
        printf("Round %d\n", turnID);
        if (turnID == 1) {
            //srand(time(NULL));
            srand(0);
            int first = rand() % 7;
            logger[0][0]["block"] = first;
            logger[0][0]["color"] = 0;
            logger[0][1]["block"] = first;
            logger[0][1]["color"] = 1;
        }
        string si0 = "in_" + to_string(turnID) + "_0.txt";
        string si1 = "in_" + to_string(turnID) + "_1.txt";
        makeInput(0, si0);
        makeInput(1, si1);
        string so0 = "out_" + to_string(turnID) + "_0.txt";
        string so1 = "out_" + to_string(turnID) + "_1.txt";
        system((string(argv[1]) + " < " + si0 + " > " + so0).c_str());
        system((string(argv[2]) + " < " + si1 + " > " + so1).c_str());
        Json::Value o0, o1;
        o0 = getOutput(so0);
        o1 = getOutput(so1);
        logger[turnID][0] = o1;
        logger[turnID][1] = o0;
        bool wa0, wa1;
        wa0 = !gameBoard.place(0, logger[turnID - 1][0]["block"].asInt(), o0["y"].asInt(), o0["x"].asInt(), o0["o"].asInt());
        ++gameBoard.typeCountForColor[0][logger[turnID - 1][0]["block"].asInt()];
        wa1 = !gameBoard.place(1, logger[turnID - 1][1]["block"].asInt(), o1["y"].asInt(), o1["x"].asInt(), o1["o"].asInt());
        ++gameBoard.typeCountForColor[1][logger[turnID - 1][1]["block"].asInt()];
        wa0 = wa0 || gameBoard.typeCountError(1);
        wa1 = wa1 || gameBoard.typeCountError(0);
        if (wa0) {
            puts("player 0 error");
            winner = 1;
        }
        if (wa1) {
            puts("player 1 error");
            if (winner != 1)
                winner = 0;
            else
                winner = -2;
        }
        if (wa0 || wa1)
            break;
        gameBoard.eliminate(0);
        gameBoard.eliminate(1);
        int wa = gameBoard.transfer();
        if (wa != -1) {
            winner = 1 - wa;
            break;
        }
        wa0 = !gameBoard.canPut(0, o1["block"].asInt());
        wa1 = !gameBoard.canPut(1, o0["block"].asInt());
        if (wa0 && wa1) {
            winner = gameBoard.elimTotal[0] - gameBoard.elimTotal[1];
            if (winner == 0)
                winner = -2;
            else
                winner = (winner < 0 ? 1 : 0);
        }
        else if (wa0)
            winner = 1;
        else if (wa1)
            winner = 0;
        if (wa0 || wa1)
            break;
        if (turnID == 300) {
            winner = -2;
            break;
        }
        ++turnID;
    }

    ofstream fout(argv[3]);
    Json::StyledWriter writer;
    Json::Value output;
    for (int i = 1; i <= turnID; ++i) {
        auto &o = output[i - 1]["output"];
        o["command"] = "request";
        auto &d = o["display"];
        d["0"]["block"] = logger[i - 1][0]["block"];
        d["1"]["block"] = logger[i - 1][1]["block"];
        if (i != turnID) {
            auto &d2 = output[i]["output"]["display"];
            d2["0"]["route"][0] = logger[i][1];
            d2["0"]["route"][0].removeMember("block");
            d2["1"]["route"][0] = logger[i][0];
            d2["1"]["route"][0].removeMember("block");
        }
    }
    auto &o = output[turnID]["output"];
    o["command"] = "finish";
    o["display"]["result"] = (winner >= 0 ? 1 - winner : winner);
    fout << writer.write(output);
    fout.close();
    return 0;
}
