#ifndef GAME_H
#define GAME_H

#include "jsoncpp/json.h"
#include <vector>

#define MAPWIDTH 10
#define MAPHEIGHT 20

class Tetris;

class GameBoard
{
public:
    // 我所在队伍的颜色（0为红，1为蓝，仅表示队伍，不分先后）
    int currBotColor;
    int enemyColor;
    int currBlockType;
    int enemyType;

    // 记录地图状态，0为空，1为以前放置，2为刚刚放置，负数为越界
    // （2用于在清行后将最后一步撤销再送给对方）
    int gridInfo[2][MAPHEIGHT + 2][MAPWIDTH + 2] = {};

    // 代表分别向对方转移的行
    int trans[2][4][MAPWIDTH + 2] = {};

    // 转移行数
    int transCount[2] = {};

    // 运行eliminate后的当前高度
    int maxHeight[2] = {};

    // 总消去行数的分数之和
    int elimTotal[2] = {};

    // 给对应玩家的各类块的数目总计
    int typeCountForColor[2][7] = {};

protected:
    // 围一圈护城河
    void init();

public:

    GameBoard();

    // 检查能否从场地顶端直接落到当前位置
    bool checkDirectDropTo(int color, int blockType, int x, int y, int o);

    // 消去行
    void eliminate(int color);

    // 转移双方消去的行，返回-1表示继续，否则返回输者
    int transfer();

    // 颜色方还能否继续游戏
    bool canPut(int color, int blockType);

    void play(const Json::Value &);

    bool place(int id, int blockType, int x, int y, int o); // 放置新块，返回是否可以放置。不检查可达性

    void deplace(int id, int blockType, int x, int y, int o); // 取消放置新块

    std::vector<Tetris> &getPlaces(int id, int blockType, std::vector<Tetris> &ans);
};

// 一次性消去行数对应分数
extern const int elimBonus[5];

extern const int blockShape[7][4][8];
// 7种形状(长L| 短L| 反z| 正z| T| 直一| 田格)，4种朝向(上左下右)，8:每相邻的两个分别为x，y

extern const int blockHeight[7][4];  // 7种形状4种朝向的块的高度

extern const int blockHalfHeight[7][4]; // 7种形状4种朝向的中心到底部的距离

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

    // 取消放置方块
    void deplace();

    // 检查能否逆时针旋转自己到o
    bool rotation(int o);
};

#endif
