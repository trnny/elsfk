#ifndef __FUUU__
#define __FUUU__

/**
 * 该文件: 
 * 解析消息
 * 给ws注册事件，用于处理消息
 */

#include "wsserver.hpp"
#include "mysql/mysqlfuuu.hpp"


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
#define _WSONCB_ [&ws](HDL hdl, const pb_map& data)

/**
 * 新加入一个用户
 */
int new_user(const string& un, const string& pw) {
    // TO-DO 从redis读取ucount、max_uid
    static int ucount = 1, max_uid = 10000;
    int retry = 6, u_id;
    while (-- retry) {
        u_id = rand() % 100 + ucount;
        if (sql.query("select * from elsfk_users where u_id=" + std::to_string(u_id))) {
            if (sql.affected_rows() == 0) {
                // u_id不存在 新增
                if (sql.query("insert into elsfk_users (u_id, u_name, u_passwd) values (" + std::to_string(u_id) + ", `" + un +"`, `" + pw + "`)"))
                    break;
                return 0;   // 有错误
            }
            continue;   // 重试
        }
        return 0;   // 有错误
    }
    ++ ucount;
    if (retry) {    // 获取成功
        if (u_id > max_uid) {
            max_uid = u_id;
        }
    }
    u_id = ++ max_uid;
    // TO-DO 更新redis
    return u_id;
}

// 给ws注册事件     ws是否需要弄成全局变量?  似乎一个ws服务器就够了
void ws_on(WS &ws) {

    // signup  注册
    ws.on("signup", _WSONCB_{
        string uname;
        string password;
        CITER iter;
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        bool ok_back = true;
        string tips = "注册成功!";
        msg_back.set_desc("signup");
        
        iter = data.find("uname");
        if (iter != data.cend()) {
            if (!getpbo(iter->second, uname) || uname.length() == 0 || uname.length() > 24) {
                ok_back = false;
                tips = "用户名不合格!";
            }
        }else{
            ok_back = false;
            tips = "用户名丢失!";
        }
        
        iter = data.find("password");
        if (iter != data.cend()) {
            if (!getpbo(iter->second, password) || password.length() != 32) {
                ok_back = false;
                tips = "密码不合格!";
            }
        }else{
            ok_back = false;
            tips = "密码丢失!";
        }
        
        if (ok_back) {
            // 消息合格
            int u_id = new_user(uname, password);
            if (u_id) {
                data_back->insert(mp("uid", makepbv(u_id)));
            }else{
                ok_back = false;
                tips = "服务器繁忙，请稍后再试!";
            }
        }
        data_back->insert(mp("ok", makepbv(ok_back)));
        data_back->insert(mp("msg", makepbv(tips)));
        string buff_back;
        msg_back.SerializeToString(&buff_back);
        ws.send(hdl, buff_back);
        cout << "[SIGNUP] " << uname << endl;
    });

    // signin  登陆
    ws.on("signin", _WSONCB_{
        int uid;
        string password;
        CITER iter;
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        bool ok_back = true;
        string tips = "登陆成功!";
        msg_back.set_desc("signin");

        iter = data.find("uid");
        if (iter != data.cend()) {
            if (!getpbo(iter->second, uid)) {
                ok_back = false;
                tips = "用户名不合格!";
            }
        }else{
            ok_back = false;
            tips = "用户名丢失!";
        }

        iter = data.find("password");
        if (iter != data.cend()) {
            if (!getpbo(iter->second, password) || password.length() != 32) {
                ok_back = false;
                tips = "密码不合格!";
            }
        }else{
            ok_back = false;
            tips = "密码丢失!";
        }

        if(ok_back) {
            // 消息合格
            if (sql.query("select u_passwd from elsfk_users where u_id=" + std::to_string(uid))) {
                if (sql.affected_rows() == 0) {
                    ok_back = false;
                    tips = "用户id不存在!";
                }else{
                    if(sql.result(tab)) {
                        if(tab[0][0] != password) {
                            ok_back = false;
                            tips = "密码错误!请重试";
                        }
                    }else{
                        ok_back = false;
                        tips = "未知错误~";
                    }
                }
            }else{
                ok_back = false;
                tips = "服务器繁忙，请稍后再试!";
            }
        }

        if(ok_back) {
            // 登陆成功  获取rid，以便后面重连
            data_back->insert(mp("rid", makepbv(ws.addHDLAndUid(uid, hdl))));
            // TO-DO  记录登陆操作 暂时未知如何获取ip
            if (!sql.query("insert into elsfk_login (u_id) values (" + std::to_string(uid) + " )")){
                printf("[WS WARNING] 用户%d登陆成功，但写入数据库失败!\n", uid);
            }
        }
        data_back->insert(mp("ok", makepbv(ok_back)));
        data_back->insert(mp("msg", makepbv(tips)));
        string buff_back;
        msg_back.SerializeToString(&buff_back);
        ws.send(hdl, buff_back);
        cout << "[SIGNIN] " << uid << endl;
    });

    // 重连
    ws.on("userrelink", _WSONCB_{
        // uid: int, rid: int
        int uid, rid;
    });

    ws.on("logout", _WSONCB_{
        CITER iter;
        Msg msg_back;
        auto data_back = msg_back.mutable_data();
        bool ok_back = true;
        string tips = "登出成功!";
        msg_back.set_desc("signin");

        auto info = ws.getInfoByHdl(hdl);
        if (NULL == info) {
            ok_back = false;
            tips = "未知用户!";
        }else{

        }

    });

    ws.on("userrename", _WSONCB_{

    });

    ws.on("roomjoin", _WSONCB_{

    });
    ws.on("gamectrl", _WSONCB_{

    });
    ws.on("friendadd", _WSONCB_{

    });
    ws.on("friendaddacc", _WSONCB_{

    });
    ws.on("frienddel", _WSONCB_{

    });
    ws.on("friendlist", _WSONCB_{

    });
    ws.on("recordlist", _WSONCB_{

    });
    ws.on("recordinfo", _WSONCB_{

    });
    ws.on("recordget", _WSONCB_{

    });
    ws.on("", _WSONCB_{

    });
    ws.on("", _WSONCB_{

    });
};






#endif