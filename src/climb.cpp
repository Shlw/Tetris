#include <cstdio>
#include <cstring>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
using namespace std;
void journal(double *p, int np, double mean);

int resource = 3;
mutex runMutex;
condition_variable runCV;

void run(double *p, int np, int *ans, int num)
{
    {
        unique_lock<mutex> runLock(runMutex);
        if (resource > 0)
            --resource;
        else {
            runCV.wait(runLock, [&] {return resource > 0;});
            --resource;
        }
    }
    /*char cmd[100];
    sprintf(cmd, "echo %g %g %g %g %g %g | ./mkdata > out%d.txt", p1, p2, q1, q2, q3, k1, num);
    system(cmd);
    FILE *file = fopen(("out" + to_string(num) + ".txt").c_str(), "r");
    fscanf(file, "%d", ans);
    fclose(file);*/
    FILE *file;
    file = fopen(("in" + to_string(num) + ".txt").c_str(), "w");
    for (int i = 0; i < np; ++i)
        fprintf(file, "%g ", p[i]);
    fclose(file);
    system(("./mkdata < in" + to_string(num) + ".txt > out" + to_string(num) + ".txt").c_str());
    file = fopen(("out" + to_string(num) + ".txt").c_str(), "r");
    fscanf(file, "%d", ans);
    fclose(file);
    {
        unique_lock<mutex> runLock(runMutex);
        ++resource;
        runCV.notify_one();
    }
}

double mean(double *p, int np)
{
    const int mean_num = 20;
    int sum = 0;
    vector<thread> vec;
    int num[mean_num];
    for (int i = 0; i < mean_num; ++i) {
        thread t(run, p, np, num + i, i);
        vec.push_back(move(t));
    }
    for (auto &t : vec)
        t.join();
    for (int i = 0; i < mean_num; ++i)
        sum += num[i];
    double ans = (double)sum / mean_num;
    journal(p, np, ans);
    return ans;
}
void journal(double *p, int np, double mean)
{
    putchar('(');
    for (int i = 0; i < np; ++i)
        printf("%g ", p[i]);
    printf(") = %g\n", mean);
    if (mean >= 58) {
        FILE *file = fopen("climb.log", "a");
        fprintf(file, "(");
        for (int i = 0; i < np; ++i)
            fprintf(file, "%g ", p[i]);
        fprintf(file, ") = %g\n", mean);
        fclose(file);
    }
}
int main()
{
    srand(getpid() * time(0));
    const int np = 6;
    double p[np];
    double mean_now = 0;
    while (mean_now < 50) {
        for (int i = 0; i < np; ++i)
            p[i] = 2 + 10.0 * rand() / RAND_MAX;
        mean_now = mean(p, np);
    }

    double step = 1.5;
    const double rate = 0.9;
    int cnt_step = 0;
    const int cnt_mx = 10;

    while (step > 0.05) {
        const int tries = 30;
        double mean_next = 0;
        //journal(mean_now);
        double nextp[np];
        for (int T = 0; T < tries; ++T) {
            double p2[np];
            for (int i = 0; i < np; ++i)
                p2[i] = p[i] + (rand() % 3 - 1) * step;
            double mean_step = mean(p2, np);
            if (mean_step > mean_next) {
                mean_next = mean_step;
                memcpy(nextp, p2, sizeof(p));
            }
        }
        puts("-----------------------------");
        if (mean_next >= mean_now) {
            mean_now = mean_next;
            memcpy(p, nextp, sizeof(p));
            ++cnt_step;
        }
        else
            cnt_step = cnt_mx;
        if (cnt_step == cnt_mx) {
            cnt_step = 0;
            step *= rate;
            printf("step:%g\n", step);
        }
    }
    return 0;
}
