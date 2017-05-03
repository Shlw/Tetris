#include <cstdlib>
#include <cstdio>
#include <string>
#include <algorithm>
#include <thread>
#include <mutex>
#include <vector>
using namespace std;

mutex mtx;

void func(int num)
{
    char cmd1[50], cmd2[50];
    sprintf(cmd1, "./mkdata > data%d.txt", num);
    sprintf(cmd2, "cat data%d.txt >> data.txt", num);
    for (int i = 0; i < 200; ++i) {
        system(cmd1);
        mtx.lock();
        system(cmd2);
        mtx.unlock();
    }
}

int main()
{
    vector<thread> v;
    for (int i = 0; i < 8; ++i)
        v.push_back(thread(func, i));
    for (auto &t : v)
        t.join();
    return 0;
}
