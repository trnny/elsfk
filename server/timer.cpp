#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <map>
#include <vector>
#include "timer.h"


int tid = 0;
struct cbms{
    FT  cb;
    uint ms;    // 剩余时间
    uint it;    // 重复 0表示不重复
};

itimerval it;
std::map<int, cbms> dl;
bool running = false;
void start() {
    if (running) return;
    running = true;
    it.it_interval.tv_usec = 1000;
    it.it_value.tv_usec = 1000;
    setitimer(ITIMER_REAL, &it, NULL);
}
void timerHandler(int signo) {
    static std::vector<FT> cbs;
    cbs.clear();
    for (auto iter = dl.begin(); iter != dl.end();) {
        if (--iter->second.ms == 0) {
            cbs.push_back(iter->second.cb);
            if (iter->second.it == 0)
                iter = dl.erase(iter);
            else {
                iter->second.ms = iter->second.it;
                ++iter;
            }
        }
        else
            ++iter;
    }
    for (FT cb : cbs)
        cb();
    if (dl.empty()) {
        it.it_value.tv_usec = 0;
        it.it_interval.tv_usec = 0;
        running = false;
    }
}
void timerInit(){
    if (tid) return;
    tid = 1;
    signal(SIGALRM, timerHandler);
}
int setTimeout(FT callback, uint ms) {
    dl[++tid] = {callback, ms, 0};
    start();
    return tid;
}
int setInterval(FT callback, uint ms) {
    dl[++tid] = {callback, ms, ms};
    start();
    return tid;
}
void clearTimeout(int timeoutId) {
    auto iter = dl.find(timeoutId);
    if (iter != dl.cend()) 
        dl.erase(iter);
}
void clearInterval(int intervalId) {
    auto iter = dl.find(intervalId);
    if (iter != dl.cend()) 
        dl.erase(iter);
}