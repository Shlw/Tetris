#include "game.h"
#include <cstdio>
#include <cassert>
#include <vector>
#include <fstream>
using namespace std;

GameBoard gameBoard;
Json::Value val;
int turnID;

void printInfo()
{
    printf("Round %d:  %d vs %d\n", turnID, gameBoard.elimTotal[0], gameBoard.elimTotal[1]);
    printf("count:");
    for (int i = 0; i < 7; ++i)
        printf(" %d", gameBoard.typeCountForColor[0][i]);
    printf("\ncount:");
    for (int i = 0; i < 7; ++i)
        printf(" %d", gameBoard.typeCountForColor[1][i]);
    putchar('\n');
    putchar('\n');
    static char ans[20][26] = {};
    for (int i = 1; i <= 20; ++i)
        sprintf(ans[i - 1], "%2d", 21 - i);
    for (int i = 0; i < 20; ++i)
        ans[i][2] = ans[i][13] = ans[i][24] = '|';
    static const char mp[3] = {' ', '*', '+'};
    for (int i = 1; i <= 20; ++i)
        for (int j = 1; j <= 10; ++j)
            ans[20 - i][2 + j] = mp[gameBoard.gridInfo[0][i][j]];
    for (int i = 1; i <= 20; ++i)
        for (int j = 1; j <= 10; ++j)
            ans[20 - i][13 + j] = mp[gameBoard.gridInfo[1][i][j]];
    for (int i = 0; i < 20; ++i)
        puts(ans[i]);
    printf("  ");
    for (int i = 0; i < 23; ++i)
        putchar('-');
    printf("\n   ");
    for (int i = 1; i <= 10; ++i)
        printf("%d", i % 10);
    putchar('|');
    for (int i = 1; i <= 10; ++i)
        printf("%d", i % 10);
    putchar('\n');
    getchar();
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        puts("usage: replay log.txt");
        return 1;
    }
    ifstream fin(argv[1]);
    string str, line;
    while (getline(fin, line))
        str += line;
    fin.close();
    Json::Reader reader;
    reader.parse(str, val);

    printInfo();

    vector<int> hvout;

    for (int i = 0; i < val.size(); ++i)
        if (val[i].isMember("output"))
            hvout.push_back(i);
    for (auto it = hvout.begin(); it != hvout.end(); ++it) {
        turnID = it - hvout.begin() + 1;
        auto &o = val[*it]["output"];
        auto &d = o["display"];
        if (o["command"] == "request") {
            auto &d2 = val[*(it + 1)]["output"]["display"];
            auto &r0 = d2["0"]["route"];
            auto &r1 = d2["1"]["route"];
            auto &r0_ = r0[r0.size() - 1];
            auto &r1_ = r1[r1.size() - 1];
            printf("%d %d %d %d\n", d["1"]["block"].asInt(), r0_["y"].asInt(), r0_["x"].asInt(), r0_["o"].asInt());
            gameBoard.place(0, d["1"]["block"].asInt(), r0_["y"].asInt(), r0_["x"].asInt(), r0_["o"].asInt());
            gameBoard.place(1, d["0"]["block"].asInt(), r1_["y"].asInt(), r1_["x"].asInt(), r1_["o"].asInt());
            printInfo();
            gameBoard.eliminate(0);
            gameBoard.eliminate(1);
            gameBoard.transfer();
            printInfo();
        }
        else if (o["command"] == "finish") {
            int lose = d["result"].asInt();
            printf("winner is %d\n", lose >= 0 ? 1 - lose : lose);
        }
    }
    return 0;
}
