#ifndef __TIMER__
#define __TIMER__


/**
 * 该文件提供定时器接口
 */


#include <functional>
using std::function;


template <typename T>
class Timer {
    typedef unsigned int uint;
    typedef function<T> FT;
public:
    // TO-DO 实现这些函数
    int setTimeout(FT callback, uint ms) {
    }
    int setInterval(FT callback, uint ms) {
    }
    void clearTimeout(int timeoutId) {
    }
    void clearInterval(int intervalId) {
    }
};


#endif