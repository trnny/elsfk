#ifndef __FUUU__
#define __FUUU__

/**
 * 该文件: 
 * 解析消息
 * 给ws注册事件，用于处理消息
 */

#include "wsserver.hpp"
#include "mysql/mysqlfuuu.hpp"
#include "game.hpp"


Msg_VType makepbv(const vector<int>& o) {
    Msg_VType v;
    for (int i = 0; i < o.size(); i++) {
        v.set_vr_int(i, o[i]);
    }
    v.set_which(5);
    return v;
}

Msg_VType makepbv(const vector<string>& o) {
    Msg_VType v;
    for (int i = 0; i < o.size(); i++) {
        v.set_vr_str(i, o[i]);
    }
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

Msg_VType makepbv(const string& o, bool bin = false) {
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

bool getpbo(const Msg_VType& v, vector<int>& o) {
    if (v.which() == 5) {
        o.resize(v.vr_int_size());
        for(int i = 0;i<v.vr_int_size(); i++) {
            o[i] = v.vr_int(i);
        }
        return true;
    }
    return false;
}

bool getpbo(const Msg_VType& v, vector<string>& o) {
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

bool getpbo(const Msg_VType& v, string& o) {
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

MyMysql sql;
vector<vector<string>> tab;
GameRoomManager roomManager;
#define _WSONCB_ [&ws](HDL hdl, const pb_map& data)

/**
 * 新加入一个用户
 */
int new_user(const string& un, const string& pw) {
    // TO-DO 从redis读取ucount、max_uid
    static int ucount = 2, max_uid = 10084;
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
    // TO-DO 更新redis
    return u_id;
}

// 给ws注册事件     ws是否需要弄成全局变量?  似乎一个ws服务器就够了
void ws_on(WS &ws) {

    /**
     * 告知所有人 匹配成功 等待大家点确定
     */
    roomManager.onMatch = [&](GameRoom* room) {
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("matchingSucceed");
        data_back->insert(mp("uids", makepbv(room->uids)));
        string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int uid : room->uids) 
            ws.send(uid, buff_back);
    };

    /**
     * 告知所有人 有人没点确定 取消匹配
     */
    roomManager.onCancel = [&](GameRoom* room) {
        Msg msg_back;
        msg_back.set_desc("matchingCancel");
        string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int uid : room->uids) 
            ws.send(uid, buff_back);
    };

    /**
     * 告知所有人 大家都点了确定 游戏开始了
     */
    roomManager.onPlay = [&](GameRoom* room) {
        vector<char> nxn;   // 用于记录
        vector<int> unn;    // 发往客户端
        unsigned char now, next, mix;
        for (int uid : room->uids) {
            now = rand() % 16;
            next = rand() % 16;
            mix = now * 16 + next;
            room->record.push(uid); // 记录四个uid
            unn.push_back(uid);
            unn.push_back(now);
            unn.push_back(next);
            nxn.push_back(mix);
        }
        for (char c : nxn) {    // 记录四个mix  mix由now和next组成
            room->record.push(c);
        }
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("gameStart");
        data_back->insert(mp("unn", makepbv(unn)));
        string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int uid : room->uids) 
            ws.send(uid, buff_back);
    };

    /**
     * 告知所有人  大家都阵亡了 游戏结束了
     */
    roomManager.onOver = [&](GameRoom* room) {
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("gameOver");
        data_back->insert(mp("ok", makepbv(true)));
        data_back->insert(mp("uid", makepbv(0)));   // 0表示大家都结束了
        string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int uid : room->uids) 
            ws.send(uid, buff_back);
    };

    // signup  注册
    ws.on("signup", _WSONCB_{
        string uname, password;
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
        string tips = "注册成功!";
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
        string buff_back;
        msg_back.SerializeToString(&buff_back);
        ws.send(hdl, buff_back);
    });

    // signin  登陆
    ws.on("signin", _WSONCB_{
        int uid;
        string password;
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
        string tips = "登陆成功!";
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
            // 登陆成功  获取rid，以便后面重连
            data_back->insert(mp("rid", makepbv(ws.addHDLAndUid(uid, hdl))));
            if (!sql.query("insert into elsfk_login (u_id) values (" + std::to_string(uid) + " )")){
                cout << "[WS WARNING] 用户[" << uid << "]登陆成功，但写入数据库失败!" << endl;
            }
        }
        data_back->insert(mp("ok", makepbv(ok)));
        data_back->insert(mp("msg", makepbv(tips)));
        string buff_back;
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
        string tips = "重连成功!";
        if (!ws.updateHDL(hdl, uid, rid)) {
            ok = false;
            tips = "重连失败";
        }
        msg_back.set_desc("userrelink");
        data_back->insert(mp("ok", makepbv(ok)));
        data_back->insert(mp("msg", makepbv(tips)));
        string buff_back;
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
        string buff_back;
        msg_back.SerializeToString(&buff_back);
        ws.send(hdl, buff_back);
    });
    
    // 匹配请求
    ws.on("matching", _WSONCB_{
        if (!roomManager.getIntoRoom(ws.getInfoByHdl(hdl)->uid))
            return;
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("matching");
        data_back->insert(mp("ok", makepbv(true)));
        string buff_back;
        msg_back.SerializeToString(&buff_back);
        ws.send(hdl, buff_back);
    });

    // 匹配确认
    ws.on("matchingSure", _WSONCB_{
        int uid = ws.getInfoByHdl(hdl)->uid;
        if (!roomManager.confirmEntry(uid))
            return;
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("matching");
        data_back->insert(mp("ok", makepbv(true)));
        data_back->insert(mp("uid", makepbv(uid)));
        string buff_back;
        msg_back.SerializeToString(&buff_back);
        auto uids = roomManager.getRoomById(uid)->uids;
        for (int uid : uids)
            ws.send(uid, buff_back);
    });

    // 请求一个新的下路块
    ws.on("newDropping", _WSONCB_{
        int uid = ws.getInfoByHdl(hdl)->uid;
        GameRoom* room = roomManager.getRoomById(uid);
        if (room == NULL || room->status != GameRoom::playing)
            return;
        int next = rand() % 19;
        // TO-DO  记录
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("newDropping");
        data_back->insert(mp("ok", makepbv(true)));
        data_back->insert(mp("uid", makepbv(uid)));
        data_back->insert(mp("next", makepbv(next)));
        string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int uid : room->uids)
            ws.send(uid, buff_back);
    });
    ws.on("reduce", _WSONCB_{
        vector<int> reduce;
        CITER iter;
        iter = data.find("reduce");
        if (iter == data.cend())
            return;
        if (!getpbo(iter->second, reduce))
            return;
        // TO-DO 记录
        int uid = ws.getInfoByHdl(hdl)->uid;
        GameRoom* room = roomManager.getRoomById(uid);
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        msg_back.set_desc("reduce");
        data_back->insert(mp("ok", makepbv(true)));
        data_back->insert(mp("uid", makepbv(uid)));
        data_back->insert(mp("reduce", makepbv(reduce)));
        string buff_back;
        msg_back.SerializeToString(&buff_back);
        for (int uid : room->uids)
            ws.send(uid, buff_back);
    });
    ws.on("blockSet", _WSONCB_{
        
    });
    ws.on("blockRm", _WSONCB_{

    });
    ws.on("gameOver", _WSONCB_{

    });
};






#endif