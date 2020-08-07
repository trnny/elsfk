#ifndef __TIMER__
#define __TIMER__


/**
 * 该文件提供定时器接口
 */

#include <functional>

/**
 * 定时器的hanlder类型
 */
typedef std::function<void(void)> FT;

/**
 * 指定msTime毫秒后执行handler
 * 返回定时器标号
 */
int setTimeout(FT handler, uint msTime);
/**
 * 每隔msTime毫秒执行一次handler
 * 返回定时器标号
 */
int setInterval(FT handler, uint msTime);
/**
 * 将定时器关闭
 */
void clearTimeout(int timeoutId);
/**
 * 将定时器关闭
 */
void clearInterval(int intervalId);

#endif