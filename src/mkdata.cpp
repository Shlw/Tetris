#include "util.h"
#include "game.h"
#include "decide.h"
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <queue>
#include <utility>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
using namespace std;

GameBoard gb;
int turn;

void printInfo(GameBoard &gb)
{
    printf("Round %d:  %d vs %d\n", turn, gb.elimTotal[0], gb.elimTotal[1]);
    printf("count:");
    for (int i = 0; i < 7; ++i)
        printf(" %d", gb.typeCountForColor[0][i]);
    printf("\ncount:");
    for (int i = 0; i < 7; ++i)
        printf(" %d", gb.typeCountForColor[1][i]);
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
            ans[20 - i][2 + j] = mp[gb.gridInfo[0][i][j]];
    for (int i = 1; i <= 20; ++i)
        for (int j = 1; j <= 10; ++j)
            ans[20 - i][13 + j] = mp[gb.gridInfo[1][i][j]];
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

int putables(Board a, int c[MAPHEIGHT + 2][MAPWIDTH + 2])
{
    static GameBoard gb2;
    memcpy(gb2.gridInfo[0], a.grid, sizeof(a.grid));
    int ans = 0;
    for (int i = 0; i < 7; ++i) {
        vector<Tetris> loc;
        gb2.getPlaces(0, i, loc);
        for (auto t : loc)
            for (int k = 0; k < 4; k++) {
                int tmpX = t.blockX + t.shape[t.orientation][2 * k];
                int tmpY = t.blockY + t.shape[t.orientation][2 * k + 1];
                c[tmpX][tmpY]++;
            }
    }
    return ans;
}
const int pnum = 6;
double p[pnum];
double F(double x) {return p[0] * x;}
double G(double x) {return -10 * p[1] * exp(-0.1 * p[2] * x) + 0.05 * p[3] * x;}
double H(double x) {return x < 2.5 ? 0 : p[4] * exp(0.03 * p[5] * x);}
int get_deep(Board &a, int maxheight, int lowest)
{
    int h = maxheight;
    while (h >= lowest) {
        int sp[MAPWIDTH], nsp = 0;
        for (int i = 1; i <= MAPWIDTH; ++i)
            if (a[h][i])
                sp[nsp++] = i;
        int l = 0;
        while (l < nsp && sp[l] == l + 1)
            ++l;
        if (l != nsp && sp[l] != MAPWIDTH - nsp + l + 1)
            break;
        --h;
    }
    return h - lowest + 1;
}
double Eval(Board a, const Block &bl)
{
    a.place(bl);
    a.eliminate();

    queue< pair<int, int> > q;
    static bool vis[MAPHEIGHT + 2][MAPWIDTH + 2];
    memset(vis, 0, sizeof(vis));
    for (int i = 1; i <= MAPWIDTH; ++i)
        if (a[MAPHEIGHT][i] == 0) {
            q.push(make_pair(MAPHEIGHT, i));
            vis[MAPHEIGHT][i] = true;
        }
    int lowest = MAPHEIGHT + 1;
    while (!q.empty()) {
        auto fr = q.front();
        q.pop();
        lowest = min(lowest, fr.first);
        for (int i = 0; i < 4; ++i) {
            static const int dir[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
            pair<int, int> tmp(fr.first + dir[i][0], fr.second + dir[i][1]);
            if (!vis[tmp.first][tmp.second] && a[tmp.first][tmp.second] == 0) {
                vis[tmp.first][tmp.second] = true;
                q.push(tmp);
            }
        }
    }
    int bad[MAPHEIGHT + 2] = {};
    int cnt[100] = {};
    int badlines = 0;
    static int c[MAPHEIGHT + 2][MAPWIDTH + 2];
    memset(c, 0, sizeof(c));
    putables(a, c);
    int sum_cnt = 0;
    for (int i = 1; i <= MAPHEIGHT; ++i)
        for (int j = 1; j <= MAPWIDTH; ++j)
            if (a[i][j] == 0)
                if (a[i - 1][j] || a[i + 1][j] == 1) {
                    if (c[i][j] == 0)
                        bad[i] = 1;
                    if (vis[i][j]) {
                        cnt[c[i][j]]++;
                        ++sum_cnt;
                    }
                }
    for (int i = 1; i <= MAPHEIGHT; ++i)
        for (int j = 1; j <= MAPWIDTH && !bad[i]; ++j)
            if (a[i][j] == 0 && !vis[i][j])
                bad[i] = 1;
    for (int i = 1; i <= MAPHEIGHT; ++i)
        if (bad[i])
            ++badlines;
    int maxheight = 0;
    for (int i = MAPHEIGHT; i >= 1; --i) {
        int cnt = 0;
        for (int j = 1; j <= MAPWIDTH; ++j)
            if (a[i][j])
                ++cnt;
        if (cnt) {
            maxheight = i;
            break;
        }
    }
    int deep = get_deep(a, maxheight, lowest);

    double ans = -30 * F(badlines);
    for (int i = 0; i < 100; ++i)
        ans += cnt[i] * G(i) * 5 / sum_cnt;
    ans += H(deep);

    static GameBoard gb2;
    memcpy(gb2.gridInfo[0], a.grid, sizeof(a.grid));
    printf("bad=%d,cnt={", badlines);
    for (int i = 0; i <= 40; ++i)
        printf("[%d]=%d,", i, cnt[i]);
    printf("},deep=%d\neval=%g\n", deep, ans);
    printInfo(gb2);

    return ans;
}

double myplace(int this_bl_type, int &finalX, int &finalY, int &finalO)
{
    std::vector<Tetris> loc;
    gb.getPlaces(0, this_bl_type, loc);
    int i;
    double best_val = -1e9, now_val;
    Block now_bl, best_bl = Block(-1, -1, -1, -1);
    Board myBoard(0, gb);
    for (i = 0; i < loc.size(); i++) {
        now_bl = Block(loc[i]);
        now_val = Eval(myBoard, now_bl);
        if (now_val > best_val) {
            best_val = now_val;
            best_bl = now_bl;
        }
    }
    finalX = best_bl.x;
    finalY = best_bl.y;
    finalO = best_bl.o;
    return best_val;
}

int main()
{
    for (int i = 0; i < pnum; ++i)
        scanf("%lf", p + i);
    gb.currBotColor = 0;
    gb.enemyColor = 1;
    srand(getpid() * time(0));
    //srand(0);
    turn = 1;
    int block = 4;
    //int block = rand() % 7;
    while (gb.canPut(0, block)) {
        //printInfo(gb);
        ++gb.typeCountForColor[0][block];
        int fx, fy, fo;
        myplace(block, fx, fy, fo);
        //naive_place(gb, 0, block, evaluate2, fx, fy, fo);

        memcpy(gb.gridInfo[1], gb.gridInfo[0], sizeof(gb.gridInfo[0]));
        memcpy(gb.typeCountForColor[1], gb.typeCountForColor[0], sizeof(gb.typeCountForColor[0]));
        gb.enemyType = block;
        int b2;
        naive_jam(gb, evaluate2, b2);

        gb.place(0, block, fx, fy, fo);
        gb.eliminate(0);
        block = b2;
        ++turn;
        int maxheight = 0;
        for (int i = MAPHEIGHT; i >= 1; --i) {
            int cnt = 0;
            for (int j = 1; j <= MAPWIDTH; ++j)
                if (gb.gridInfo[0][i][j])
                    ++cnt;
            if (cnt) {
                maxheight = i;
                break;
            }
        }
        if (maxheight == MAPHEIGHT)
            break;
        if (turn % 8 == 0) {
            for (int i = maxheight + 1; i > 1; --i)
                memcpy(gb.gridInfo[0][i], gb.gridInfo[0][i - 1], sizeof(gb.gridInfo[0][0]));
            for (int i = 1; i <= MAPWIDTH; ++i)
                gb.gridInfo[0][1][i] = 1;
            gb.gridInfo[0][1][rand() % MAPWIDTH + 1] = 0;
            if (rand() % 2)
                gb.gridInfo[0][1][rand() % MAPWIDTH + 1] = 0;
        }
    }
    printf("%d\n", turn);
    return 0;
}
