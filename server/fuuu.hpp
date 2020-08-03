#ifndef __FUUU__
#define __FUUU__

/**
 * 该文件: 
 * 解析消息
 * 给ws注册事件，用于处理消息
 */

#include <time.h>
#include <sys/time.h>
#include "wsserver.hpp"
#include "mysql/mysqlfuuu.hpp"
#include "redis/redisfuuu.hpp"
#include "game.hpp"
#include "timer.h"

Msg_VType makepbv(const std::vector<int>& o) {
    Msg_VType v;
    for (int i : o)
        v.add_vr_int(i);
    v.set_which(5);
    return v;
}

Msg_VType makepbv(const std::vector<std::string>& o) {
    Msg_VType v;
    for (const std::string& s : o)
        v.add_vr_str(s);
    v.set_which(4);
    return v;
}

Msg_VType makepbv(bool o) {
    Msg_VType v;
    v.set_v_bool(o);
    v.set_which(3);
    return v;
}

Msg_VType makepbv(int o) {
    Msg_VType v;
    v.set_v_int(o);
    v.set_which(2);
    return v;
}

Msg_VType makepbv(const std::string& o, bool bin = false) {
    Msg_VType v;
    if (!bin) {
        v.set_v_str(o);
        v.set_which(1);
    }
    else
    {
        v.set_v_bytes(o);
        v.set_which(6);
    }
    return v;
}

Msg_VType makepbv(const char* o) {
    Msg_VType v;
    v.set_v_str(o);
    v.set_which(1);
    return v;
}

bool getpbo(const Msg_VType& v, std::vector<int>& o) {
    if (v.which() == 5) {
        o.resize(v.vr_int_size());
        for(int i = 0;i<v.vr_int_size(); i++) {
            o[i] = v.vr_int(i);
        }
        return true;
    }
    return false;
}

bool getpbo(const Msg_VType& v, std::vector<std::string>& o) {
    if (v.which() == 4) {
        o.resize(v.vr_str_size());
        for(int i = 0;i<v.vr_str_size(); i++) {
            o[i] = v.vr_str(i);
        }
        return true;
    }
    return false;
}

bool getpbo(const Msg_VType& v, bool& o) {
    if (v.which() == 3) {
        o = v.v_bool();
        return true;
    }
    return false;
}

bool getpbo(const Msg_VType& v, int& o) {
    if (v.which() == 2) {
        o = v.v_int();
        return true;
    }
    return false;
}

bool getpbo(const Msg_VType& v, std::string& o) {
    if (v.which() == 1) {
        o = v.v_str();
        return true;
    }else if(v.which() == 6)
    {
        o = v.v_bytes();
        return true;
    }
    return false;
}

std::vector<std::vector<std::string>> tab;
// // 显示查询结果
// void pTable(const std::vector<std::vector<std::string>>& tab) {
//     for (const auto& row : tab) {
//         for (const auto& col : row) {
//             std::cout << col << "    ";
//         }
//         std::cout << std::endl;
//     }
//     std::cout << std::endl;
// }

#define _WSONCB_ [](HDL hdl, const pb_map& data)

/**
 * 新加入一个用户
 */
int new_user(const std::string& un, const std::string& pw) {
    static int ucount, max_uid = 0;
    if (max_uid == 0) {
        if (!redis.getValue("intUserCount", ucount) || !redis.getValue("intMaxUid", max_uid)) {
            std::cerr << "[ERROR] : 创建新用户时读取redis失败" << std::endl;
            return 0;
        }
    }
    int retry = 6, u_id;
    while (-- retry) {
        u_id = rand() % 100 + ucount + 10000;
        if (sql.query("select * from elsfk_users where u_id=" + std::to_string(u_id)) && sql.result(tab)) {
            if (tab.size() == 0) {
                // u_id不存在 新增   如果插入字符型 一定要加引号
                if (sql.query("insert into elsfk_users (u_id, u_name, u_passwd) values (" + std::to_string(u_id) + ", \"" + un +"\", \"" + pw + "\")"))
                    break;  // 插入成功
                return 0;   // 有错误
            }
            continue;   // 重试
        }
        return 0;   // 有错误
    }
    ++ ucount;
    if (retry) {    // 获取成功
        if (u_id > max_uid)
            max_uid = u_id;
    }
    else    // 使用max_uid后一个
        u_id = ++ max_uid;
    if (!redis.setValue("intUserCount", ucount) || !redis.setValue("intMaxUid", max_uid)) {
        std::cerr << "[ERROR] : 创建新用户时写入redis失败" << std::endl;    // 新的id已生成但是写入redis失败
    }
    return u_id;
}

/**
 * 获取时间戳 ms
 */
uint64_t get_ms() {
    static struct timespec ts;
    clock_gettime(CLOCK_BOOTTIME_ALARM, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

// 给ws注册事件
void ws_on() {

    // setInterval([]{
    //     std::cout << get_ms() << std::endl;
    // }, 5000);

    /**
     * 告知所有人 匹配成功 等待大家点确定
     * uids 4
     */
    roomManager.onMatch = [](GameRoom* room) {
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("matchingSucceed");
        data_back->insert(mp("uids", makepbv(room->uids)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int uid : room->uids)  // 长度4
            ws.send(uid, buff_back);
    };

    /**
     * 告知所有人 有人没点确定 取消匹配
     * uids 4
     */
    roomManager.onCancel = [](GameRoom* room) {
        Msg msg_back;
        msg_back.set_desc("matchingCancel");
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int uid : room->uids)  // 长度4
            ws.send(uid, buff_back);
    };

    /**
     * 告知所有人 大家都点了确定 游戏开始了
     * uids 4 -> 5
     */
    roomManager.onPlay = [](GameRoom* room) {
        std::vector<char> nxn;   // 用于记录
        std::vector<int> unn;    // 发往客户端
        std::string ustr;   // 记录数据库
        unsigned char now, next, mix;
        room->lastTime = get_ms();
        for (int uid : room->uids) {
            now = rand() % 16;
            next = rand() % 16;
            mix = now * 16 + next;
            room->record.push(uid); // 记录四个uid
            unn.push_back(uid);
            unn.push_back(now);
            unn.push_back(next);
            nxn.push_back(mix);
            ustr += ", " + std::to_string(uid);
        }
        for (char c : nxn) {    // 记录四个mix  mix由now和next组成
            room->record.push(c);
        }
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("gameStart");
        data_back->insert(mp("unn", makepbv(unn)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int uid : room->uids)  // 长度4
            ws.send(uid, buff_back);
        room->uids.push_back(setInterval([room]{
            if (get_ms() - room->lastTime > 11999) {
                std::vector<int> over;
                over.insert(over.begin(), room->uids.begin(), room->uids.begin() + 4);
                for (int uid : over)
                    roomManager.gameOver(uid);
            }
        }, 3000));
        if(sql.query("insert into elsfk_game_4 (g_level, u_id1, u_id2, u_id3, u_id4) values (0" + ustr + ")") && sql.query("select @@IDENTITY") && sql.result(tab)) {
            room->record.id = tab[0][0];
        }
    };

    /**
     * 告知所有人  大家都阵亡了 游戏结束了
     * uids n -> 4
     */
    roomManager.onOver = [](GameRoom* room) {
        clearInterval(room->uids[4]);
        room->uids.resize(4);
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("gameOver");
        data_back->insert(mp("ok", makepbv(true)));
        data_back->insert(mp("uid", makepbv(0)));   // 0表示大家都结束了
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int uid : room->uids)  // 长度4
            ws.send(uid, buff_back);
        room->record.push(-1);
        redis.setMapByField("hashGameRecords", room->record.id, room->record.getBuffer());
    };

    // signup  注册
    ws.on("signup", _WSONCB_{
        std::string uname, password;
        CITER iter;
        iter = data.find("uname");
        if (iter == data.cend())
            return;
        if (!getpbo(iter->second, uname) || uname.length() == 0 || uname.length() > 24) 
            return;        
        iter = data.find("password");
        if (iter == data.cend())
            return;
        if (!getpbo(iter->second, password) || password.length() != 32)
            return;
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        bool ok = true;
        std::string tips = "注册成功!";
        msg_back.set_desc("signup");
        int u_id = new_user(uname, password);
        if (u_id)
            data_back->insert(mp("uid", makepbv(u_id)));
        else{
            ok = false;
            tips = "服务器繁忙，请稍后再试!";
        }
        data_back->insert(mp("ok", makepbv(ok)));
        data_back->insert(mp("msg", makepbv(tips)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        ws.send(hdl, buff_back);
    });

    // signin  登陆
    ws.on("signin", _WSONCB_{
        int uid;
        std::string password;
        CITER iter;
        iter = data.find("uid");
        if (iter == data.cend())
            return;
        if (!getpbo(iter->second, uid))
            return;
        iter = data.find("password");
        if (iter == data.cend())
            return;
        if (!getpbo(iter->second, password) || password.length() != 32)
            return;
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        bool ok = true;
        std::string tips = "登陆成功!";
        msg_back.set_desc("signin");
        if (sql.query("select u_passwd from elsfk_users where u_id=" + std::to_string(uid)) && sql.result(tab)) {
            if (tab.size() == 0) {
                ok = false;
                tips = "用户id不存在!";
            }else{
                if(tab[0][0] != password) {
                    // 密码错误
                    ok = false;
                    tips = "密码错误!请重试";
                }
            }
        }else{
            // 查询错误
            ok = false;
            tips = "服务器繁忙，请稍后再试!";
        }
        if(ok) {
            HDL ohdl = ws.getHDLByUid(uid); // 被挤下线
            if (ohdl.lock()) {
                Msg temp_msg;
                temp_msg.set_desc("crowd");
                std::string temp_buff;
                temp_msg.SerializeToString(&temp_buff);
                ws.send(ohdl, temp_buff);
            }
            data_back->insert(mp("rid", makepbv(ws.addHDLAndUid(uid, hdl))));
            if (!sql.query("insert into elsfk_login (u_id) values (" + std::to_string(uid) + " )")){
                std::cout << "[WS WARNING] 用户[" << uid << "]登陆成功，但写入数据库失败!" << std::endl;
            }
            GameRoom *room = roomManager.getRoomById(uid);
            if (room) {
                data_back->insert(mp("record", makepbv(room->record.getBuffer(), true)));
            }
            
        }
        data_back->insert(mp("ok", makepbv(ok)));
        data_back->insert(mp("msg", makepbv(tips)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        ws.send(hdl, buff_back);
    });

    /**
     * 重连
     * 暂时 重连只能重连成登陆状态  并不能恢复重连前的状态
     * 后面使用room->record是可以实现恢复游戏状态的
     */
    ws.on("userrelink", _WSONCB_{
        // uid: int, rid: int
        int uid, rid;
        CITER iter;
        iter = data.find("uid");
        if (iter == data.cend())
            return;
        if (!getpbo(iter->second, uid))
            return;
        iter = data.find("rid");
        if (iter == data.cend())
            return;
        if (!getpbo(iter->second, rid))
            return;
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        bool ok = true;
        std::string tips = "重连成功!";
        if (!ws.updateHDL(hdl, uid, rid)) {
            ok = false;
            tips = "重连失败";
        }
        msg_back.set_desc("userrelink");
        data_back->insert(mp("ok", makepbv(ok)));
        data_back->insert(mp("msg", makepbv(tips)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        ws.send(hdl, buff_back);
    });

    ws.on("logout", _WSONCB_{
        if (!ws.removeHDL(hdl))
            return;
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("logout");
        data_back->insert(mp("ok", makepbv(true)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        ws.send(hdl, buff_back);
        // TO-DO 更新记录 数据库
    });
    
    // 匹配请求
    ws.on("matching", _WSONCB_{
        auto info = ws.getInfoByHdl(hdl);
        if (info == NULL)
            return;
        if (!roomManager.getIntoRoom(info->uid))
            return;
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("matching");
        data_back->insert(mp("ok", makepbv(true)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        ws.send(hdl, buff_back);
    });

    // 匹配确认
    ws.on("matchingSure", _WSONCB_{
        auto info = ws.getInfoByHdl(hdl);
        if (info == NULL)
            return;
        int uid = info->uid;
        if (!roomManager.confirmEntry(uid))
            return;
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("matchingSure");
        data_back->insert(mp("ok", makepbv(true)));
        data_back->insert(mp("uid", makepbv(uid)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        auto uids = roomManager.getRoomById(uid)->uids;
        for (int i = 0; i < 4; i++) 
            ws.send(uids[i], buff_back);
    });

    // 匹配放弃
    ws.on("playerCancel", _WSONCB_{
        auto info = ws.getInfoByHdl(hdl);
        if (info == NULL)
            return;
        if (!roomManager.getOutRoom(info->uid))
            return;
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("playerCancel");
        data_back->insert(mp("ok", makepbv(true)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        ws.send(hdl, buff_back);
    });

    // 特殊
    ws.on("newDropping", _WSONCB_{
        auto info = ws.getInfoByHdl(hdl);
        if (info == NULL)
            return;
        int uid = info->uid;
        int next = rand() % 19;
        GameRoom* room = roomManager.getRoomById(uid);
        if (room == NULL || room->status != GameRoom::playing)
            return;
        // 记录
        uint64_t now = get_ms();
        room->record.push((char)(room->id2idx[uid]));
        room->record.push((char)next);
        room->record.push((int16_t)(now - room->lastTime));
        room->lastTime = now;
        // 记录完成
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("newDropping");
        data_back->insert(mp("ok", makepbv(true)));
        data_back->insert(mp("uid", makepbv(uid)));
        data_back->insert(mp("next", makepbv(next)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int i = 0; i < 4; i++) 
            ws.send(room->uids[i], buff_back);
    });
    ws.on("droppingChanged", _WSONCB_{
        auto info = ws.getInfoByHdl(hdl);
        if (info == NULL)
            return;
        int dropping;
        CITER iter;
        iter = data.find("dropping");
        if (iter == data.cend())
            return;
        if (!getpbo(iter->second, dropping))
            return;
        int uid = info->uid;
        GameRoom* room = roomManager.getRoomById(uid);
        if (room == NULL || room->status != GameRoom::playing)
            return;
        // 记录
        uint64_t now = get_ms();
        room->record.push((char)(room->id2idx[uid]));
        room->record.push((char)(dropping + 21));
        room->record.push((int16_t)(now - room->lastTime));
        room->lastTime = now;
        // 记录完成
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("droppingChanged");
        data_back->insert(mp("ok", makepbv(true)));
        data_back->insert(mp("uid", makepbv(uid)));
        data_back->insert(mp("dropping", makepbv(dropping)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int i = 0; i < 4; i++) 
            ws.send(room->uids[i], buff_back);
    });
    // 变长
    ws.on("reduce", _WSONCB_{
        auto info = ws.getInfoByHdl(hdl);
        if (info == NULL)
            return;
        std::vector<int> reduce;
        CITER iter;
        iter = data.find("reduce");
        if (iter == data.cend())
            return;
        if (!getpbo(iter->second, reduce))
            return;
        int uid = info->uid;
        GameRoom* room = roomManager.getRoomById(uid);
        if (room == NULL || room->status != GameRoom::playing)
            return;
        // 记录
        uint64_t now = get_ms();
        room->record.push((char)(room->id2idx[uid]));
        room->record.push((char)(63 + reduce.size()));
        room->record.push((int16_t)(now - room->lastTime));
        for (int bytes : reduce)
            room->record.push(bytes);
        room->lastTime = now;
        // 记录完成
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("reduce");
        data_back->insert(mp("ok", makepbv(true)));
        data_back->insert(mp("uid", makepbv(uid)));
        data_back->insert(mp("reduce", makepbv(reduce)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int i = 0; i < 4; i++) 
            ws.send(room->uids[i], buff_back);
    });
    // 定长4
    ws.on("posChanged", _WSONCB_{
        auto info = ws.getInfoByHdl(hdl);
        if (info == NULL)
            return;
        int pos;
        CITER iter;
        iter = data.find("pos");
        if (iter == data.cend())
            return;
        if (!getpbo(iter->second, pos))
            return;
        int uid = info->uid;
        GameRoom* room = roomManager.getRoomById(uid);
        if (room == NULL || room->status != GameRoom::playing)
            return;
        // 记录
        uint64_t now  = get_ms();
        room->record.push((char)(room->id2idx[uid]));
        room->record.push((char)19);
        room->record.push((int16_t)(now - room->lastTime));
        room->record.push(pos);
        room->lastTime = now;
        // 记录完成
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("posChanged");
        data_back->insert(mp("ok", makepbv(true)));
        data_back->insert(mp("uid", makepbv(uid)));
        data_back->insert(mp("pos", makepbv(pos)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int i = 0; i < 4; i++) 
            ws.send(room->uids[i], buff_back);
    });
    // 定长0
    ws.on("gameOver", _WSONCB_{
        auto info = ws.getInfoByHdl(hdl);
        if (info == NULL)
            return;
        int uid = info->uid;
        GameRoom* room = roomManager.gameOver(uid);
        if (room == NULL)   // 最后一个结束的人不会收到自己结束 会收到整个游戏结束
            return;
        // 记录
        uint64_t now  = get_ms();
        room->record.push((char)(room->id2idx[uid]));
        room->record.push((char)20);
        room->record.push((int16_t)(now - room->lastTime));
        room->lastTime = now;
        // 记录完成
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("gameOver");
        data_back->insert(mp("ok", makepbv(true)));
        data_back->insert(mp("uid", makepbv(uid)));
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int i = 0; i < 4; i++) 
            ws.send(room->uids[i], buff_back);
    });

    /**
     * 获取记录列表
     */
    ws.on("recordList", _WSONCB_{
        auto info = ws.getInfoByHdl(hdl);
        if (info == NULL)
            return;
        int uid = info->uid;
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("recordList");
        if (sql.query("select rec_no,rec_time_start,u_id1,u_id2,u_id3,u_id4 from elsfk_game_4 where u_id1=" + std::to_string(uid) + " or u_id2=" + std::to_string(uid) + " or u_id3=" + std::to_string(uid) + " or u_id4=" + std::to_string(uid)) && sql.result(tab)){
            data_back->insert(mp("ok", makepbv(true)));
            if (tab.size()) {
                std::vector<std::string> ids, dates, uid1s, uid2s, uid3s, uid4s;
                for (const auto& item : tab) {
                    ids.push_back(item[0]);
                    dates.push_back(item[1]);
                    uid1s.push_back(item[2]);
                    uid2s.push_back(item[3]);
                    uid3s.push_back(item[4]);
                    uid4s.push_back(item[5]);
                }
                data_back->insert(mp("ids", makepbv(ids)));
                data_back->insert(mp("dates", makepbv(dates)));
                data_back->insert(mp("uid1s", makepbv(uid1s)));
                data_back->insert(mp("uid2s", makepbv(uid2s)));
                data_back->insert(mp("uid3s", makepbv(uid3s)));
                data_back->insert(mp("uid4s", makepbv(uid4s)));
            }
        }else{
            data_back->insert(mp("ok", makepbv(false)));
            data_back->insert(mp("msg", makepbv("服务器繁忙!")));
        }
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        ws.send(hdl, buff_back);
    });

    /**
     * 获取某条记录
     */
    ws.on("recordGet", _WSONCB_{
        auto info = ws.getInfoByHdl(hdl);
        if (info == NULL)
            return;
        string recordId;
        CITER iter;
        iter = data.find("recordId");
        if (iter == data.cend())
            return;
        if (!getpbo(iter->second, recordId))
            return;
        string record;
        redis.getMapValueByField("hashGameRecords", recordId, record);
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("recordGet");
        data_back->insert(mp("ok", makepbv(true)));
        data_back->insert(mp("record", makepbv(record, true))); // 以二进制发送
        std::string buff_back;
        msg_back.SerializeToString(&buff_back);
        ws.send(hdl, buff_back);
    });
};






#endif