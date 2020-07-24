#include "fuuu.hpp"




int main() {
    timerInit();
    srand((int)time(0));
    ws_on();
    ws.run();
}