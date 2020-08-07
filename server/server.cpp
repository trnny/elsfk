#include <iostream>
#include <unistd.h>
#include "timer.h"
#include "wsHandle.hpp"

using std::cout;
using std::endl;

/**
 * 测试定时器效果
 */
void timerTest() {
    int count = 0;
    int it = setInterval([&]{
        cout << "每3000ms执行一次，第" << ++count << "次执行" << endl;
        if (count >= 8) {
            clearInterval(it);
            cout << "再过5000ms程序停止运行" << endl;
            setTimeout([&]{
                cout << "时间到了，停止运行" << endl;
                exit(0);
            }, 5000);
        }
    }, 3000);
    setTimeout([&]{
        cout << "我是0ms定时器" << endl;
    }, 0);
    cout << "我是直接输出" << endl;
    setTimeout([&]{
        cout << "我也是0ms定时器" << endl;
    }, 0);

    while (true)
        sleep(0);
}

int main() {
    // timerTest();
    srand((int)time(0));
    setInterval([]{
        srand((int)time(0) + rand());
    }, 1000 * 60 * 5);  // 每5分钟更新一下种子
    ws_on();
    ws.run();
}