#include "game.h"
#include <cstring>
#include <algorithm>
#include <queue>
using namespace std;

constexpr int GameBoard::elimBonus[4];
constexpr int GameBoard::blockShape[7][4][8];

GameBoard::GameBoard()
{
    init();
}

void GameBoard::init()
{
    int i;
    for (i = 0; i < MAPHEIGHT + 2; i++) {
        gridInfo[1][i][0] = gridInfo[1][i][MAPWIDTH + 1] = -2;
        gridInfo[0][i][0] = gridInfo[0][i][MAPWIDTH + 1] = -2;
    }
    for (i = 0; i < MAPWIDTH + 2; i++) {
        gridInfo[1][0][i] = gridInfo[1][MAPHEIGHT + 1][i] = -2;
        gridInfo[0][0][i] = gridInfo[0][MAPHEIGHT + 1][i] = -2;
    }
}

bool GameBoard::checkDirectDropTo(int color, int blockType, int x, int y, int o)
{
    auto &def = blockShape[blockType][o];
    for (; x <= MAPHEIGHT; x++)
        for (int i = 0; i < 4; i++) {
            int _x = def[i * 2] + x, _y = def[i * 2 + 1] + y;
            if (_x > MAPHEIGHT)
                continue;
            if (_x < 1 || _y < 1 || _y > MAPWIDTH || gridInfo[color][_x][_y])
                return false;
        }
    return true;
}

void GameBoard::eliminate(int color)
{
    int &count = transCount[color] = 0;
    int i, j, emptyFlag, fullFlag;
    maxHeight[color] = MAPHEIGHT;
    for (i = 1; i <= MAPHEIGHT; i++) {
        emptyFlag = 1;
        fullFlag = 1;
        for (j = 1; j <= MAPWIDTH; j++) {
            if (gridInfo[color][i][j] == 0)
                fullFlag = 0;
            else
                emptyFlag = 0;
        }
        if (fullFlag) {
            for (j = 1; j <= MAPWIDTH; j++) {
                // 注意这里只转移以前的块，不包括最后一次落下的块（“撤销最后一步”）
                trans[color][count][j] = gridInfo[color][i][j] == 1 ? 1 : 0;
                gridInfo[color][i][j] = 0;
            }
            count++;
        }
        else if (emptyFlag) {
            maxHeight[color] = i - 1;
            break;
        }
        else
            for (j = 1; j <= MAPWIDTH; j++) {
                gridInfo[color][i - count][j] =
                    gridInfo[color][i][j] > 0 ? 1 : gridInfo[color][i][j];
                if (count)
                    gridInfo[color][i][j] = 0;
            }
    }
    maxHeight[color] -= count;
    if (count > 0)
        elimTotal[color] += elimBonus[count - 1];
}

int GameBoard::transfer()
{
    int color1 = 0, color2 = 1;
    if (transCount[color1] == 0 && transCount[color2] == 0)
        return -1;
    if (transCount[color1] == 0 || transCount[color2] == 0) {
        if (transCount[color1] == 0 && transCount[color2] > 0)
            swap(color1, color2);
        int h2;
        maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];
        if (h2 > MAPHEIGHT)
            return color2;
        int i, j;

        for (i = h2; i > transCount[color1]; i--)
            for (j = 1; j <= MAPWIDTH; j++)
                gridInfo[color2][i][j] = gridInfo[color2][i - transCount[color1]][j];

        for (i = transCount[color1]; i > 0; i--)
            for (j = 1; j <= MAPWIDTH; j++)
                gridInfo[color2][i][j] = trans[color1][i - 1][j];
        return -1;
    }
    else {
        int h1, h2;
        maxHeight[color1] = h1 = maxHeight[color1] + transCount[color2];//从color1处移动count1去color2
        maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];

        if (h1 > MAPHEIGHT)
            return color1;
        if (h2 > MAPHEIGHT)
            return color2;

        int i, j;
        for (i = h2; i > transCount[color1]; i--)
            for (j = 1; j <= MAPWIDTH; j++)
                gridInfo[color2][i][j] = gridInfo[color2][i - transCount[color1]][j];

        for (i = transCount[color1]; i > 0; i--)
            for (j = 1; j <= MAPWIDTH; j++)
                gridInfo[color2][i][j] = trans[color1][i - 1][j];

        for (i = h1; i > transCount[color2]; i--)
            for (j = 1; j <= MAPWIDTH; j++)
                gridInfo[color1][i][j] = gridInfo[color1][i - transCount[color2]][j];

        for (i = transCount[color2]; i > 0; i--)
            for (j = 1; j <= MAPWIDTH; j++)
                gridInfo[color1][i][j] = trans[color2][i - 1][j];

        return -1;
    }
}

bool GameBoard::canPut(int color, int blockType)
{
    Tetris t(this, blockType, color);
    for (int x = MAPHEIGHT; x >= 1; x--)
        for (int y = 1; y <= MAPWIDTH; y++)
            for (int o = 0; o < 4; o++) {
                t.set(x, y, o);
                if (t.isValid() && checkDirectDropTo(color, blockType, x, y, o))
                    return true;
            }
    return false;
}

int GameBoard::play(const Json::Value &input)
{
    int nextTypeForColor[2];
    int turnID, blockType;

    // 先读入第一回合，得到自己的颜色
    // 双方的第一块肯定是一样的
    turnID = input["responses"].size() + 1;
    auto &first = input["requests"][(Json::UInt) 0];

    blockType = first["block"].asInt();
    int currBotColor = first["color"].asInt();

    int enemyColor = 1 - currBotColor;
    nextTypeForColor[0] = blockType;
    nextTypeForColor[1] = blockType;
    typeCountForColor[0][blockType]++;
    typeCountForColor[1][blockType]++;


    // 然后分析以前每回合的输入输出，并恢复状态
    // 循环中，color 表示当前这一行是 color 的行为
    // 平台保证所有输入都是合法输入
    for (int i = 1; i < turnID; i++) {
        int currTypeForColor[2] = { nextTypeForColor[0], nextTypeForColor[1] };
        int x, y, o;
        // 根据这些输入输出逐渐恢复状态到当前回合

        // 先读自己的输出，也就是自己的行为
        // 自己的输出是一个序列，但是只有最后一步有用
        // 所以只保留最后一步
        // 然后模拟最后一步放置块
        auto &myOutput = input["responses"][i - 1];
        blockType = myOutput["block"].asInt();
        y = myOutput["x"].asInt();
        x = myOutput["y"].asInt();
        o = myOutput["o"].asInt();

        // 我当时把上一块落到了 x y o！
        Tetris myBlock(this, currTypeForColor[currBotColor], currBotColor);
        myBlock.set(x, y, o).place();

        // 我给对方什么块来着？
        typeCountForColor[enemyColor][blockType]++;
        nextTypeForColor[enemyColor] = blockType;

        // 然后读自己的输入，也就是对方的行为
        // 裁判给自己的输入是对方的最后一步
        auto &myInput = input["requests"][i];
        blockType = myInput["block"].asInt();
        y = myInput["x"].asInt();
        x = myInput["y"].asInt();
        o = myInput["o"].asInt();

        // 对方当时把上一块落到了 x y o！
        Tetris enemyBlock(this, currTypeForColor[enemyColor], enemyColor);
        enemyBlock.set(x, y, o).place();

        // 对方给我什么块来着？
        typeCountForColor[currBotColor][blockType]++;
        nextTypeForColor[currBotColor] = blockType;

        // 检查消去
        eliminate(0);
        eliminate(1);

        // 进行转移
        transfer();
    }
    return nextTypeForColor[currBotColor];
}

bool GameBoard::place(int id, int blockType, int x, int y, int o)
{
    typeCountForColor[id][blockType]++;
    Tetris tr(this, blockType, id);
    return tr.set(x, y, o).place();
}

void GameBoard::deplace(int id, int blockType, int x, int y, int o)
{
    typeCountForColor[id][blockType]--;
    Tetris tr(this, blockType, id);
    tr.set(x, y, o).deplace();
}

vector<Tetris> &GameBoard::getPlaces(int id, int blockType, vector<Tetris> &ans)
{
    queue<Tetris> q;
    static bool vis[MAPHEIGHT + 2][MAPWIDTH + 2][4];
    memset(vis, 0, sizeof(vis));
    Tetris tmp(this, blockType, id);
    for (int y = 1; y <= MAPWIDTH; ++y)
        if (tmp.set(MAPHEIGHT - 1, y, 0).isValid()) {
            q.push(tmp);
            vis[tmp.blockX][tmp.blockY][tmp.orientation] = true;
        }
    ans.clear();
    while (!q.empty()) {
        auto &fr = q.front();
        tmp.set(fr.blockX, fr.blockY, fr.orientation);
        q.pop();
        if (tmp.onGround())
            ans.push_back(tmp);
        ++tmp.blockY;
        if (!vis[tmp.blockX][tmp.blockY][tmp.orientation] && tmp.isValid()) {
            q.push(tmp);
            vis[tmp.blockX][tmp.blockY][tmp.orientation] = true;
        }
        --tmp.blockY, --tmp.blockY;
        if (!vis[tmp.blockX][tmp.blockY][tmp.orientation] && tmp.isValid()) {
            q.push(tmp);
            vis[tmp.blockX][tmp.blockY][tmp.orientation] = true;
        }
        ++tmp.blockY, --tmp.blockX;
        if (!vis[tmp.blockX][tmp.blockY][tmp.orientation] && tmp.isValid()) {
            q.push(tmp);
            vis[tmp.blockX][tmp.blockY][tmp.orientation] = true;
        }
        for (int o = 0; o < 4; ++o)
            if (!vis[tmp.blockX][tmp.blockY][o]) {
                tmp.orientation = o;
                if (tmp.isValid()) {
                    q.push(tmp);
                    vis[tmp.blockX][tmp.blockY][tmp.orientation] = true;
                }
            }
    }
    return ans;
}

Tetris::Tetris(GameBoard *gb, int t, int color) : gameBoard(gb), blockType(t), shape(GameBoard::blockShape[t]), color(color)
{}

Tetris &Tetris::set(int x, int y, int o)
{
    blockX = x == -1 ? blockX : x;
    blockY = y == -1 ? blockY : y;
    orientation = o == -1 ? orientation : o;
    return *this;
}

bool Tetris::isValid(int x, int y, int o)
{
    x = x == -1 ? blockX : x;
    y = y == -1 ? blockY : y;
    o = o == -1 ? orientation : o;
    if (o < 0 || o > 3)
        return false;

    int i, tmpX, tmpY;
    for (i = 0; i < 4; i++) {
        tmpX = x + shape[o][2 * i];
        tmpY = y + shape[o][2 * i + 1];
        if (tmpX < 1 || tmpY > MAPWIDTH ||
            tmpY < 1 || tmpX > MAPHEIGHT ||
            gameBoard->gridInfo[color][tmpX][tmpY] != 0)
            return false;
    }
    return true;
}

bool Tetris::onGround()
{
    if (isValid() && !isValid(blockX - 1))
        return true;
    return false;
}

bool Tetris::place()
{
    if (!onGround())
        return false;

    int i, tmpX, tmpY;
    for (i = 0; i < 4; i++) {
        tmpX = blockX + shape[orientation][2 * i];
        tmpY = blockY + shape[orientation][2 * i + 1];
        gameBoard->gridInfo[color][tmpX][tmpY] = 2;
    }
    return true;
}

void Tetris::deplace()
{
    int i, tmpX, tmpY;
    for (i = 0; i < 4; i++) {
        tmpX = blockX + shape[orientation][2 * i];
        tmpY = blockY + shape[orientation][2 * i + 1];
        gameBoard->gridInfo[color][tmpX][tmpY] = 0;
    }
}

bool Tetris::rotation(int o)
{
    if (o < 0 || o > 3)
        return false;

    if (orientation == o)
        return true;

    int fromO = orientation;
    while (true) {
        if (!isValid(-1, -1, orientation))
            return false;

        if (fromO == o)
            break;

        fromO = (fromO + 1) % 4;
    }
    return true;
}
