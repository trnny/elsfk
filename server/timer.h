#ifndef __TIMER__
#define __TIMER__


/**
 * 该文件提供定时器接口
 */

#include <functional>


typedef std::function<void(void)> FT;

int setTimeout(FT callback, uint ms);
int setInterval(FT callback, uint ms);
void clearTimeout(int timeoutId);
void clearInterval(int intervalId);

#endif