#ifndef __GAME__
#define __GAME__

/**
 * 该文件
 * 提供游戏相关的接口
 */

#include <set>
#include <map>
#include <vector>
#include <string>

using std::set;
using std::map;
using std::vector;
using std::string;

/**
 * 存取记录
 */
class GameRecord {
    /**
     * 记录
     * 前16字节记4个uid, 后面用0123表示这4个uid
     * 之后4字节表示4玩家初始的块
     * 再之后，每个事件由4的整数倍长度的字节保存
     * 这4字节中 1字节表示uid的标号 1字节表示事件 2个字节表示本事件与上个事件的时间间隔ms
     * 事件附带的信息跟在事件后
     */
    string buffer;
public:
    void push(int bytes) {
        static char b[4];
        *(int*)b = bytes;
        buffer.push_back(b[0]);
        buffer.push_back(b[1]);
        buffer.push_back(b[2]);
        buffer.push_back(b[3]);
    }
    void push(int16_t bytes) {
        static char b[2];
        *(int16_t*)b = bytes;
        buffer.push_back(b[0]);
        buffer.push_back(b[1]);
    }
    void push(char byte) {
        buffer.push_back(byte);
    }
};

/**
 * 一个对战房间
 */
class GameRoom {
    int uids[4];    // idx 2 id
    map<int, int> id2idx;   // 通过id找idx
    int startTime;  // 开始时间
    int delay;      // 距上次操作时间差ms
    enum Status{
        matching,
        playing,
        over
    };
    void play();    // 开始
};

/**
 * 管理房间
 */
class GameRoomManager {
    map<int, GameRoom*> rooms;
    bool isPlaying(int uid);    // 判断是否在游戏
    GameRoom* getGameRoom(int uid);
    void removeGameRoom(int uid);   // map中 四个人的都要移除
    set<GameRoom*> matching;    // 正在匹配的房间
};


#endif