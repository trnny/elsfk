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
#include <stack>
#include <functional>
#include "timer.hpp"

using std::set;
using std::map;
using std::vector;
using std::string;
using std::stack;
using std::function;

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
    const string& getBuffer() const {
        return buffer;
    };
};

/**
 * 一个对战房间
 */
struct GameRoom {
public:
    vector<int> uids;       // idx 2 id  匹配和确认等
    map<int, int> id2idx;   // 通过id找idx
    int startTime;  // 开始时间
    int lastTime;   // 上次时间
    enum Status{
        matching,   // 还在匹配中
        waiting,    // 匹配上了 还在等确认
        cancel,     // 时间到了 有人没确认 取消等下一批人匹配
        playing,    // 
        over
    } status;       // 房间的状态
    GameRecord record;  // 房间的对战记录
};

/**
 * 管理房间
 */
class GameRoomManager {
    Timer<void(void)> timer;
    map<int, GameRoom*> rooms;      // 通过uid找room  有matching、waiting、playing三种
    set<GameRoom*> matching;        // 正在匹配的房间  有matching、waiting、cancel三种
public:
    /**
     * 处理函数
     * onMatch  匹配成功  等待确定
     * onCancel  超时未确定  房间被取消
     * onPlay   玩家均确定  开始游戏
     * onOver  玩家均阵亡  游戏结束
     */
    function<void(GameRoom*)> onMatch = NULL, onCancel = NULL, onPlay = NULL, onOver = NULL;
    /**
     * 获取房间信息
     */
    GameRoom* getRoomById(int uid){
        auto iter = rooms.find(uid);
        if (iter == rooms.cend()) 
            return NULL;
        return iter->second;
    }
    /**
     * uid请求加入房间
     * 返回加入是否成功
     * 房间人齐触发onMatch
     * 超时未全部确认触发onCancel
     */
    bool getIntoRoom(int uid) {
        if (getRoomById(uid)) return false;
        for (GameRoom* room : matching) {
            if (room->status == GameRoom::cancel) {
                room->uids.push_back(uid);
                room->id2idx[uid] = 0;
                room->status = GameRoom::matching;
                rooms[uid] = room;
                return true;
            }
            if (room->status == GameRoom::matching) {
                room->id2idx[uid] = room->uids.size();
                room->uids.push_back(uid);
                rooms[uid] = room;
                if (room->uids.size() == 4) {
                    room->status = GameRoom::waiting;
                    if (onMatch) 
                        onMatch(room);
                    // TO-DO 定时器还未实现 其实无法取消
                    room->uids.push_back(timer.setTimeout([&]{
                        room->uids.resize(4);
                        room->status = GameRoom::cancel;
                        if (onCancel) 
                            onCancel(room);
                        for (int uid : room->uids)
                            rooms.erase(uid);
                        room->uids.clear();
                        room->id2idx.clear();
                    }, 10000));
                }
                return true;
            }
        }
        GameRoom* room = new GameRoom();
        matching.insert(room);
        room->uids.push_back(uid);
        room->id2idx[uid] = 0;
        room->status = GameRoom::matching;
        rooms[uid] = room;
        return true;
    }
    /**
     * uid确认进入游戏
     * 全部确认触发onPlay
     */
    bool confirmEntry(int uid) {
        GameRoom* room = getRoomById(uid);
        if (room == NULL || room->status != GameRoom::waiting) return false;
        for (int i = 5; i < room->uids.size(); i++)
            if (uid == room->uids[i]) 
                return false;
        room->uids.push_back(uid);
        if (room->uids.size() >= 9) {
            timer.clearTimeout(room->uids[4]);
            room->uids.resize(4);
            room->status = GameRoom::playing;
            if (onPlay)
                onPlay(room);
            matching.erase(room);
        }
        return true;
    }
    /**
     * uid阵亡
     * 全部阵亡触发onOver
     */
    bool gameOver(int uid) {
        GameRoom* room = getRoomById(uid);
        if (room == NULL || room->status != GameRoom::playing) return false;
        for (int i = 4; i < room->uids.size(); i++) 
            if (uid == room->uids[i])
                return false;
        room->uids.push_back(uid);
        if (room->uids.size() >= 8) {
            room->uids.resize(4);
            room->status = GameRoom::over;
            if (onOver) 
                onOver(room);
            for (int uid : room->uids)
                rooms.erase(uid);
            delete room;
        }
        return true;
    }
};


#endif