#include <cstdio>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
using namespace std;
void journal(double p1, double p2, double q1, double q2, double q3, double k1, double mean);

int resource = 8;
mutex runMutex;
condition_variable runCV;

void run(double p1, double p2, double q1, double q2, double q3, double k1, int *ans, int num)
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
    fprintf(file, "%g %g %g %g %g %g\n", p1, p2, q1, q2, q3, k1);
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

double mean(double p1, double p2, double q1, double q2, double q3, double k1)
{
    const int mean_num = 20;
    int sum = 0;
    vector<thread> vec;
    int num[mean_num];
    for (int i = 0; i < mean_num; ++i) {
        thread t(run, p1, p2, q1, q2, q3, k1, num + i, i);
        vec.push_back(move(t));
    }
    for (auto &t : vec)
        t.join();
    for (int i = 0; i < mean_num; ++i)
        sum += num[i];
    double ans = (double)sum / mean_num;
    journal(p1, p2, q1, q2, q3, k1, ans);
    return ans;
}
void journal(double p1, double p2, double q1, double q2, double q3, double k1, double mean)
{
    printf("(%g %g %g %g %g %g) = %g\n", p1, p2, q1, q2, q3, k1, mean);
    if (mean > 130) {
        FILE *file = fopen("climb.log", "a");
        fprintf(file, "(%g %g %g %g %g %g) = %g\n", p1, p2, q1, q2, q3, k1, mean);
        fclose(file);
    }
}
int main()
{
    srand(getpid() * time(0));
    double p1, p2, q1, q2, q3, k1;
    double mean_now = 0;
    while (mean_now < 110) {
        p1 = 5.0 * rand() / RAND_MAX;
        p2 = 5.0 * rand() / RAND_MAX;
        q1 = 5.0 * rand() / RAND_MAX;
        q2 = 5.0 * rand() / RAND_MAX;
        q3 = 5.0 * rand() / RAND_MAX;
        k1 = 5.0 * rand() / RAND_MAX;
        mean_now = mean(p1, p2, q1, q2, q3, k1);
    }

    double step = 0.5;
    const double rate = 0.8;
    int cnt_step = 0;
    const int cnt_mx = 10;

    while (step > 0.01) {
        //journal(mean_now);
        double np1, np2, nq1, nq2, nq3, nk1;
        double mean_next = 0;
        for (int i1 = -1; i1 <= 1; ++i1)
            for (int i2 = -1; i2 <= 1; ++i2)
                for (int i3 = -1; i3 <= 1; ++i3)
                    for (int i4 = -1; i4 <= 1; ++i4)
                        for (int i5 = -1; i5 <= 1; ++i5)
                            for (int i6 = -1; i6 <= 1; ++i6) {
                                double _p1, _p2, _q1, _q2, _q3, _k1;
                                _p1 = p1 + i1 * step;
                                _p2 = p2 + i2 * step;
                                _q1 = q1 + i3 * step;
                                _q2 = q2 + i4 * step;
                                _q3 = q3 + i5 * step;
                                _k1 = k1 + i6 * step;
                                double mean_step = mean(_p1, _p2, _q1, _q2, _q3, _k1);
                                if (mean_step > mean_next) {
                                    mean_next = mean_step;
                                    np1 = p1, np2 = p2, nq1 = q1, nq2 = q2, nq3 = q3, nk1 = k1;
                                }
                            }
        if (mean_next >= mean_now) {
            mean_now = mean_next;
            p1 = np1, p2 = np2, q1 = nq1, q2 = nq2, q3 = nq3, k1 = nk1;
            ++cnt_step;
        }
        else
            cnt_step = cnt_mx;
        if (cnt_step == cnt_mx) {
            cnt_step = 0;
            step *= rate;
        }
    }
    return 0;
}
