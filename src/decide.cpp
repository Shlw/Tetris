#include "decide.h"
#include <algorithm>
#include <iostream>
#include <cstring>
#include <ctime>
#define DEPTH_LIM 3
#define debug(x) std::cerr << #x << "=" << x << std::endl
using namespace std;
#define N 55
#define M 55
#define K 105

const double eps = 1e-9;
int dcmp(double x)
{
    if (x > eps)
        return 1;
    else if (x < -eps)
        return -1;
    else
        return 0;
}

class Standard_Simplex //标准型，输入，输出和对偶时使用标准型，要用算法时转成松弛型
{
  public:
    int n, m; //m 个约束中没算大于等于零的约束
    double b[M], c[N];
    double a[M][N]; //m 行，n 列
    Standard_Simplex()
    {
        memset(a,0,sizeof(a));
        memset(b,0,sizeof(b));
        memset(c,0,sizeof(c));
    }
    void duality()  //转对偶，小心下标不要因为 n,m 不同而越界
    {
        int i, j;
        swap(n, m);
        for (i = 1; i <= max(n, m); i++) //非常暴力的全都翻转一遍
            for (j = i + 1; j <= max(n, m); j++)
                swap(a[i][j], a[j][i]);
    }
};

class Slash_Simplex //这是一个时间空间都很浪费的实现方法，矩阵大小为 (n+m)*(n+m) ,松弛型 ,也可称为 Augmented form
{
  public:
    int n, m, k;          // n 个变量, m 个约束 (在松弛型下则为 n+m 个变量)
    double b[K], c[K], v; /*
                          命名同算导，b 是约束，当 i 是基本变量时，b_i 为他的方程的常数项 ,否则可能为一些随机值（不是零!!!）
                          c 是目标函数系数（对于 i 为基本变量， c_i = 0，这个是清零了的！)*/
    double a[K][K];       //a_ij 只在 i 为基本变量， j 为非基本变量时才有意义, 否则可能为一些随机值（不是零!!!）
    bool basic[K];        //表示该变量是否为基本变量

    Slash_Simplex() {}
    Slash_Simplex(const Standard_Simplex &sim)
    {
        memset(a, 0, sizeof(a));
        memset(b, 0, sizeof(b));
        memset(c, 0, sizeof(c));
        n = sim.n;
        m = sim.m;
        k = n + m;
        v = 0;
        int i, j;
        for (i = n + 1; i <= n + m; i++)
            b[i] = sim.b[i - n];
        for (j = 1; j <= n; j++)
            c[j] = sim.c[j];
        for (j = n + 1; j <= n + m; j++)
            c[j] = 0;
        for (i = 1; i <= n + m; i++)
        {
            if (i <= n)
                basic[i] = 0;
            else
                basic[i] = 1;
        }
        for (i = n + 1; i <= k; i++)
            for (j = 1; j <= n; j++)
                a[i][j] = sim.a[i - n][j];
    }
    void initLaux(const Slash_Simplex &sim) //设置Laux，不同于算导，这里将新的变量编号为 (n + m + 1)
    {
        *this = sim;
        memset(c, 0, sizeof(c));
        for (int i = n + 1; i <= k; i++)
            a[i][k + 1] = -1;
        n++, k++;
        c[k] = -1;
        basic[k] = 0;
    }

    void Print()
    {
        int i, j;
        cerr << "Call Print:" << endl;
        //printf("Call Print:");
        cerr << n << ' ' << m << ' ' << k << endl;
        //printf("%d %d %d\n", n, m, k);
        for (i = 1; i <= k; i++)
            cerr << b[i] << " ";
            //printf("%.4lf ", b[i]);
        cerr << endl;
        for (i = 1; i <= k; i++)
            cerr << c[i] << " ";
            //printf("%.4lf ", c[i]);
        cerr << endl;
        for (i = 1; i <= k; i++)
            cerr << basic[i] << " ";
            //printf("%d ", basic[i]);
        cerr << endl;
        for (i = 1; i <= k; i++)
        {
            for (j = 1; j <= k; j++)
                cerr << a[i][j] << " ";
                //printf("%.4lf ", a[i][j]);
            cerr << endl;
        }
    }

    void pivot(int l, int e) //把 e 变成基本变量（即 e 是替入变量)
    {
        //先计算关于 x_e 的方程的新系数
        int i, j;
        b[e] = b[l] / a[l][e];
        b[l] = 0;
        for (j = 1; j <= k; j++)
            if (!basic[j] && j != e)
                a[e][j] = a[l][j] / a[l][e];
            else
                a[e][j] = 0;
        a[e][l] = 1.0 / a[l][e];
        //再计算其他方程的新系数
        for (i = 1; i <= k; i++)
            if (basic[i] && i != l)
            {
                b[i] = b[i] - a[i][e] * b[e];
                for (j = 1; j <= k; j++)
                    if (!basic[j] && j != e)
                        a[i][j] = a[i][j] - a[i][e] * a[e][j];
                a[i][l] = -a[i][e] * a[e][l];
            }
        //再算新的目标函数系数
        v = v + c[e] * b[e];
        for (j = 1; j <= k; j++)
            if (!basic[j] && j != e)
                c[j] = c[j] - c[e] * a[e][j];
        c[l] = -c[e] * a[e][l];
        c[e] = 0; //e 不再是非基本变量，清零一下

        basic[e] = 1;
        basic[l] = 0;
    }

    int get_pivot_index() //判断是否还有在目标函数中系数为正的非基础变量，有则返回序号最小的(bland rule)，没有则返回 0
    {
        for (int j = 1; j <= k; j++)
            if (!basic[j] && dcmp(c[j]) == 1)
                return j;
        return 0;
    }

    double rel;
    int simplex(bool is_init) //结果存到 rel 里，返回 -1 表示无解， 返回 0 表示正常，返回 1 表示无界
    {
        if (!is_init) //假如是从is_init里调用的该函数，则不用判断合法性
            if (!initialize_simplex())
                return -1;
        //Print();
        int i, e, l;
        while (e = get_pivot_index())
        {
            //debug(e);
            double del = 1e12;
            for (i = 1; i <= k; i++)
                if (basic[i])
                    if (dcmp(a[i][e]) == 1)
                    {
                        if (b[i] / a[i][e] < del)
                            l = i, del = b[i] / a[i][e];
                    }
            if (del == 1e12)
            {
                //debug(e);
                //debug(c[e]);
                //Print();
                //while(1);
                return 1; //无界
            }
            else
                pivot(l, e);
        }
        rel = v;
        return 0;
    }

    bool initialize_simplex() //只会在第一次调用simplex函数时调用一次。可行的话，将初始值设好，且返回1，无可行解则返回0
    {
        int min_i, i, j;
        double min_b = 1e9;
        for (i = n + 1; i <= k; i++)
            if (min_b > b[i])
                min_b = b[i], min_i = i;
        if (min_b >= -eps)
            return 1; //初始基本解已经合法了

        Slash_Simplex *Laux = new Slash_Simplex(); //为了防止爆栈
        Laux->initLaux(*this);
        Laux->pivot(min_i, Laux->k);
        Laux->simplex(1);
        if (dcmp(Laux->rel) == 0)
        {
            if (Laux->basic[k+1]) //注意是 k+1,不要用混掉
                for (j = 1; j <= k; j++)
                    if (!Laux->basic[j] && dcmp(Laux->a[k+1][j]) != 0)
                    {
                        Laux->pivot(k+1, j);
                        break;
                    }
            memcpy(a, Laux->a, sizeof(a));
            memcpy(b, Laux->b, sizeof(b));
            memcpy(basic, Laux->basic, sizeof(basic));
            for (i = 1; i <= k; i++)
                if (basic[i]) //把原先目标函数中变成基本变量的用它等式右边的东西换回去
                {
                    for (j = 1; j <= k; j++)
                        if (!basic[j])
                            c[j] -= c[i] * a[i][j];
                    v += b[i] * c[i];
                    c[i] = 0;
                }
            delete Laux;
            return 1;
        }
        else
        {
            delete Laux;
            return 0;
        }
    }
};


std::function<double(Board, const Block &)> Eval;
int nouse;
void set_Eval(std::function<double(Board, const Block &)> EFun)
{
    Eval = EFun;
}

int global_i; //for debug

double naive_place(GameBoard &gameBoard, int this_col, int this_bl_type)
{
    std::vector<Tetris> loc;
    gameBoard.getPlaces(this_col, this_bl_type, loc);
    int i, best_ch;
    double best_val = -1e9, now_val;
    Block now_bl, best_bl;
    Board myBoard(this_col, gameBoard);

    //debug(loc.size());
    for (i = 0; i < loc.size(); i++)
    {
        now_bl = Block(loc[i]);
        now_val = Eval(myBoard, now_bl);
        /*
        if(global_i == 30 && i == 29)
        {
            debug(i);
            debug(now_val);
            debug(loc[i].blockX);
            debug(loc[i].blockY);
            debug(loc[i].orientation);
        }*/
        if (now_val > best_val)
        {
            best_val = now_val;
            best_bl = now_bl;
            best_ch = i;
        }
    }
    //debug(best_ch);
    return best_val;
}

double Place_Turn(int dep, GameBoard& gameBoard, int pl_col, int this_bl_type, int &finalX = nouse, int &finalY = nouse, int &finalO = nouse, int &blockFE = nouse) //floc 表示最终选的块的位置,blockFE表示给敌人的块
{
    if (dep == DEPTH_LIM) //最后一层直接贪
        return naive_place(gameBoard, pl_col, this_bl_type);
    //debug(dep);
    std::vector<Tetris> loc;
    gameBoard.getPlaces(pl_col, this_bl_type, loc);
    int i, j, n, m;
    vector<vector<double> > A(loc.size());
    vector<int> index;
    GameBoard nowBoard;

    for (i = 0; i < loc.size(); i++)
    {
        global_i = i;
        //debug(i);
        //debug(loc[i].blockX);
        //debug(loc[i].blockY);
        //debug(loc[i].orientation);
        Board myBoard(pl_col, gameBoard);
        Block now_bl = Block(loc[i]);
        double now_val = Eval(myBoard, now_bl);
        //debug(now_val);
        nowBoard = gameBoard;
        nowBoard.place(pl_col, this_bl_type, loc[i].blockX, loc[i].blockY, loc[i].orientation);
        nowBoard.eliminate(pl_col);
        index = Jam_Turn(dep + 1, nowBoard, pl_col, A[i]);
    }
    n = loc.size();
    m = A[0].size();

    //debug(n);
    //debug(m);
    /*
    for(i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
            cerr << A[i][j] << " ";
        cerr << endl;
    }*/
    Standard_Simplex ss;
    ss.n = n + 2;
    ss.m = m + 2;
    ss.c[n+1] = 1;
    ss.c[n+2] = -1;
    for(i = 1; i <= m; i++)
    {
        ss.b[i] = 0;
        for(j = 1; j <= n; j++)
            ss.a[i][j] = -A[j-1][i-1]; //注意下标要-1
        ss.a[i][n+1] = 1;
        ss.a[i][n+2] = -1;
    }   
    ss.b[m+1] = 1;
    for(j = 1; j <= n; j++)
        ss.a[m+1][j] = 1;
    ss.b[m+2] = -1;
    for(j = 1; j <= n; j++)
        ss.a[m+2][j] = -1;

    Slash_Simplex as(ss);
    //as.Print();
    int zt = as.simplex(0);
    //as.Print();
    //debug(zt);
    //debug(as.rel);
    if(dep != 1) //不是第一层就不用考虑决策的事情
        return as.rel;

    //决策过程
    srand(gameBoard.turnID * time(0));
    double p1[45], p2[45], rd, sum;
    int ch1, ch2;
    for (i = 1; i <= n; i++)
    {
        if(as.basic[i])
            p1[i] = as.b[i];
        else p1[i] = 0;
        //cerr << p1[i] << " ";
    }
    //cerr << endl;
    for (i = 1; i <= m; i++) //得到对偶的答案
    {
        if(!as.basic[i+n+2])
            p2[i] = -as.c[i+n+2];
        else p2[i] = 0;
        //cerr << p2[i] << " ";
    }
    //cerr << endl;

    sum = 0;
    rd = (double)(rand()%10000) / 10000.0;
    //debug(rd);
    for (i = 1; i <= n; i++)
        if(sum + p1[i] > rd)
        {
            ch1 = i - 1;
            break;
        }
        else sum += p1[i];
    finalX = loc[ch1].blockX;
    finalY = loc[ch1].blockY;
    finalO = loc[ch1].orientation;
    //debug(ch1);

    sum = 0;
    rd = (double)(rand()%10000) / 10000.0;
    for (i = 1; i <= m; i++)
        if(sum + p2[i] > rd)
        {
            ch2 = i - 1;
            break;
        }
        else sum += p2[i];
    blockFE = index[ch2];
}

vector<int> Jam_Turn(int dep, GameBoard &gameBoard, int pl_col, vector<double> &A_i) //返回可行方案编号
{
    int  min_type = 1e9;
    vector<int> index;
    for (int i = 0; i < 7; i++)
        min_type = std::min(gameBoard.typeCountForColor[pl_col][i], min_type);
    for (int i = 0; i < 7; i++)
        if (gameBoard.typeCountForColor[pl_col][i] <= min_type + 1)
        {
            gameBoard.typeCountForColor[pl_col][i] ++;
            A_i.push_back(Place_Turn(dep + 1, gameBoard, pl_col, i));
            index.push_back(i);
            gameBoard.typeCountForColor[pl_col][i] --;
        }
    return index;
}


double naive_place2(GameBoard &gameBoard, int this_col, int this_bl_type, std::function<double(Board, const Block&)> Eval, int &finalX, int &finalY, int &finalO)
{
    std::vector<Tetris> loc;
    gameBoard.getPlaces(this_col, this_bl_type, loc);
    int i;
    double best_val = -1e9, now_val;
    Block now_bl, best_bl;
    Board myBoard(this_col, gameBoard);
    for (i = 0; i < loc.size(); i++)
    {
        now_bl = Block(loc[i]);
        now_val = Eval(myBoard, now_bl);
        if(now_val > best_val)
        {
            best_val = now_val;
            best_bl = now_bl;
        }
    }
    finalX = best_bl.x;
    finalY = best_bl.y;
    finalO = best_bl.o;
    return best_val;
}

/*
void naive_jam(GameBoard &gameBoard, std::function<double(Board, const Block&)> Eval, int &blockForEnemy)
{
    int f_x, f_y, f_o, min_type = 1e9, now_col;
    naive_place(gameBoard, gameBoard.enemyColor, gameBoard.enemyType, Eval, f_x, f_y, f_o);
    gameBoard.place(gameBoard.enemyColor, gameBoard.enemyType, f_x, f_y, f_o);
    gameBoard.eliminate(gameBoard.enemyColor);
    for (int i = 0; i < 7; i++)
        min_type = std::min(gameBoard.typeCountForColor[gameBoard.enemyColor][i], min_type);
    double now_val, worst_val = 1e9;
    int worst_bl;
    for (int i = 0; i < 7; i++)
    {
        if (gameBoard.typeCountForColor[gameBoard.enemyColor][i] <= min_type + 1)
        {
            now_val = naive_place(gameBoard, gameBoard.enemyColor, i, Eval, f_x, f_y, f_o);
            if(now_val < worst_val)
            {
                worst_val = now_val;
                worst_bl = i;
            }
        }
    }
    blockForEnemy = worst_bl;
}
*/