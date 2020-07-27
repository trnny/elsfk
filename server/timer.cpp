#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <map>
#include <vector>
#include "timer.h"


int _id = 0;
struct _cbms{
    FT  cb;
    uint ms;
    uint it;
};

itimerval _it;
std::map<int, _cbms> _dl;
bool _running = false;
void _start() {
    if (_running) return;
    _running = true;
    _it.it_interval.tv_usec = 1000;
    _it.it_value.tv_usec = 1000;
    setitimer(ITIMER_REAL, &_it, NULL);
}
void _handler(int signo) {
    static std::vector<FT> cbs;
    cbs.clear();
    for (auto iter = _dl.begin(); iter != _dl.end();) {
        if (--iter->second.ms == 0) {
            cbs.push_back(iter->second.cb);
            if (iter->second.it == 0)
                iter = _dl.erase(iter);
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
    if (_dl.empty()) {
        _it.it_value.tv_usec = 0;
        _it.it_interval.tv_usec = 0;
        _running = false;
    }
}

int setTimeout(FT callback, uint ms) {
    ms == 0 && (ms = 1);
    _dl[_id] = {callback, ms, 0};
    _start();
    return _id++;
}
int setInterval(FT callback, uint ms) {
    ms == 0 && (ms = 1);
    _dl[_id] = {callback, ms, ms};
    _start();
    return _id++;
}
void clearTimeout(int timeoutId) {
    auto iter = _dl.find(timeoutId);
    if (iter != _dl.cend()) 
        _dl.erase(iter);
}
void clearInterval(int intervalId) {
    auto iter = _dl.find(intervalId);
    if (iter != _dl.cend()) 
        _dl.erase(iter);
}

class _T{   // 用来初始化timer
public:
    _T() {
        if (_id) return;
        _id = 1;
        signal(SIGALRM, _handler);
    }
}_t;