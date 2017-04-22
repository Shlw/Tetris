#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

const int mapheight=20;
const int mapwidth=10; 

template<class T> T sqr(T x){return x*x;}

double evaluate(int a[mapheight+2][mapwidth+2]){
    bool reachable[mapheight+2][mapwidth+2];
    int reachlength[mapheight+2][mapwidth+2]{};
    int xmax=0;
    for (int i=mapheight;i>0;--i)
        for (int j=1;j<=mapwidth;++j){
            if (a[i][j]>0 && !xmax) xmax=i;
            if (a[i][j]>0 || reachlength[i+1][j]<0) reachlength[i][j]=-1;
                else reachlength[i][j]=reachlength[i+1][j]+1;
            reachable[i][j]=reachlength[i][j]>0;
        }

    double ret=0;

    for (int i=xmax;i>0;--i){
        int j=1;
        double tret=0,bonus=sqrt(xmax-i+1);
        while (j<=mapwidth){
            if (a[i][j]) {++j; continue;}
            double cnt=0;
            int prej=j;
            while (!a[i][j]) ++j,++cnt;
            cnt=sqrt(cnt);
            for (int k=prej;k<j;++k) if (!reachable[i][k]) cnt*=bonus;
            tret+=cnt;
        }
        ret+=(xmax-i+1)*tret;
    }

    return ret;
}
