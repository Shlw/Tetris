#ifndef GAME_H
#define GAME_H

#include "jsoncpp/json.h"

#define MAPWIDTH 10
#define MAPHEIGHT 20

class GameBoard
{
public:
    // 记录地图状态，0为空，1为以前放置，2为刚刚放置，负数为越界
    // （2用于在清行后将最后一步撤销再送给对方）
    int gridInfo[2][MAPHEIGHT + 2][MAPWIDTH + 2] = { 0 };

    // 代表分别向对方转移的行
    int trans[2][4][MAPWIDTH + 2] = { 0 };

    // 转移行数
    int transCount[2] = { 0 };

    // 运行eliminate后的当前高度
    int maxHeight[2] = { 0 };

    // 总消去行数的分数之和
    int elimTotal[2] = { 0 };

    // 给对应玩家的各类块的数目总计
    int typeCountForColor[2][7] = { 0 };

protected:
    // 围一圈护城河
    void init();

public:
    // 一次性消去行数对应分数
    static constexpr int elimBonus[4] = { 1, 3, 5, 7 };

    static constexpr int blockShape[7][4][8] = {
        { { 0, 0, 0, 1, 0, -1, -1, -1 }, { 0, 0, 1, 0, -1, 0, -1, 1 }, { 0, 0, 0, -1, 0, 1, 1, 1 }, { 0, 0, -1, 0, 1, 0, 1, -1 } },
        { { 0, 0, 0, -1, 0, 1, -1, 1 }, { 0, 0, -1, 0, 1, 0, 1, 1 }, { 0, 0, 0, 1, 0, -1, 1, -1 }, { 0, 0, 1, 0, -1, 0, -1, -1 } },
        { { 0, 0, 0, 1, -1, 0, -1, -1 }, { 0, 0, 1, 0, 0, 1, -1, 1 }, { 0, 0, 0, -1, 1, 0, 1, 1 }, { 0, 0, -1, 0, 0, -1, 1, -1 } },
        { { 0, 0, 0, -1, -1, 0, -1, 1 }, { 0, 0, -1, 0, 0, 1, 1, 1 }, { 0, 0, 0, 1, 1, 0, 1, -1 }, { 0, 0, 1, 0, 0, -1, -1, -1 } },
        { { 0, 0, 0, -1, 1, 0, 0, 1 }, { 0, 0, -1, 0, 0, -1, 1, 0 }, { 0, 0, 0, 1, -1, 0, 0, -1 }, { 0, 0, 1, 0, 0, 1, -1, 0 } },
        { { 0, 0, -1, 0, 1, 0, 2, 0 }, { 0, 0, 0, 1, 0, -1, 0, -2 }, { 0, 0, 1, 0, -1, 0, -2, 0 }, { 0, 0, 0, -1, 0, 1, 0, 2 } },
        { { 0, 0, 1, 0, 0, -1, 1, -1 }, { 0, 0, 0, -1, -1, 0, -1, -1 }, { 0, 0, -1, 0, 0, 1, -1, 1 }, { 0, 0, 0, 1, 1, 0, 1, 1 } }
    };// 7种形状(长L| 短L| 反z| 正z| T| 直一| 田格)，4种朝向(上左下右)，8:每相邻的两个分别为x，y

    GameBoard();

    // 检查能否从场地顶端直接落到当前位置
    bool checkDirectDropTo(int color, int blockType, int x, int y, int o);

    // 消去行
    void eliminate(int color);

    // 转移双方消去的行，返回-1表示继续，否则返回输者
    int transfer();

    // 颜色方还能否继续游戏
    bool canPut(int color, int blockType);

    int play(const Json::Value &); // 返回BlockType
};

class Tetris
{
public:
    GameBoard *gameBoard;
    const int blockType;   // 标记方块类型的序号 0~6
    int blockX;            // 旋转中心的x轴坐标
    int blockY;            // 旋转中心的y轴坐标
    int orientation;       // 标记方块的朝向 0~3
    const int(*shape)[8]; // 当前类型方块的形状定义

    int color;

    Tetris(GameBoard *gb, int t, int color);

    Tetris &set(int x = -1, int y = -1, int o = -1); // return self

    // 判断当前位置是否合法
    bool isValid(int x = -1, int y = -1, int o = -1);

    // 判断是否落地
    bool onGround();

    // 将方块放置在场地上
    bool place();

    // 检查能否逆时针旋转自己到o
    bool rotation(int o);
};

#endif
