#ifndef __WSSERVER__
#define __WSSERVER__


/**
 * 该文件
 * 提供ws通信的接口
 * 提供hdl的管理接口
 */

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include "msg.pb.h"
#include "game.hpp"
#include "timer.h"

typedef websocketpp::server<websocketpp::config::asio> server;
typedef google::protobuf::Map<std::string, Msg_VType> pb_map;
typedef google::protobuf::MapPair<std::string, Msg_VType> mp;
typedef google::protobuf::Map<std::string, Msg_VType>::const_iterator CITER;
typedef google::protobuf::Map<std::string, Msg_VType>::iterator ITER;
typedef websocketpp::connection_hdl HDL;

class WS
{
public:
    struct HDLInfo
    {
        int uid = 0;    // 用户id
        int rid = 0;    // 用户随机标识  用于重连认证
        int tmo = 0;    // 超时标记  用于移除重连等待
        HDLInfo (int u, int r) {
            uid = u;
            rid = r;
        }
    };
private:
    typedef std::function<void(HDL, const pb_map&)> efbk;
    std::map<std::string, std::vector<efbk>> ee;
    server mep;
    void emit(const std::string& en, HDL t1, const pb_map& t2) {
        auto iter = ee.find(en);
        if (iter != ee.cend())
            for (auto ef : iter->second)
                ef(t1, t2);
    }
    struct cmp_hdl
    {
        bool operator()(HDL hdl1, HDL hdl2) const
        {
            if (hdl1.lock() != hdl2.lock())
                return hdl1.lock() < hdl2.lock();
            return false;
        }
    };

    std::map<HDL, HDLInfo*, cmp_hdl> hdl2info;      // 通过hdl访问用户的信息
    std::map<int, HDL> uid2hdl;                     // 通过uid找到hdl
    std::set<HDLInfo*> rll;                         // 重连队列 应该用list
    int timerId = 0;    // 定时检查重连队列的定时器  重连队列空的时候不检查
    /**
     * 定时器启动
     */
    void timerOn() {
        if (timerId) return;
        timerId = setInterval([&]{
            std::vector<HDLInfo*> toDel; // 需要被删除的
            for (HDLInfo* info : rll)
                if (info->tmo < 1000) 
                    toDel.push_back(info);
                else
                    info->tmo -= 1000;
            for (HDLInfo* info : toDel) {
                rll.erase(info);
                delete info;
            }
            if (rll.empty()) {
                clearInterval(timerId);
                timerId = 0;
            }
        }, 1000);
    }

public:
    WS() {
        mep.set_error_channels(websocketpp::log::elevel::all);
        mep.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);
        mep.init_asio();
        mep.set_message_handler([this](HDL hdl, server::message_ptr msg_ptr) {
            if(msg_ptr->get_opcode() == websocketpp::frame::opcode::binary) {
                Msg msg;
                if (msg.ParseFromString(msg_ptr->get_payload())) {
                    emit(msg.desc(), hdl, msg.data());      // 将消息分发到对应的handler
                }
                else
                    std::cout << "[MSG PARSE ERROR]" << std::endl;
            }
            else{
                std::cout << "[UNKOWN MSG] " << std::endl;
            }
        });
        mep.set_close_handler([this](HDL hdl) {
            auto iter = hdl2info.find(hdl);
            if (iter != hdl2info.cend()) {
                roomManager.getOutRoom(iter->second->uid);  // 1 在断开时检查退出房间
                iter->second->tmo = 15000;
                rll.insert(iter->second);
                uid2hdl.erase(iter->second->uid);
                hdl2info.erase(iter);
                timerOn();  // 启动计时器
            }
        });
    }
    ~WS() {
        mep.stop();
    }
    /**
     * 发送二进制消息
     * 目前，应该是发送pb序列化结果
     */
    void send(HDL hdl, const std::string& s) {
        if (hdl.lock())
            mep.send(hdl, s, websocketpp::frame::opcode::binary);
    }
    /**
     * 向uid发送
     */
    void send(int uid, const std::string& s) {
        send(getHDLByUid(uid), s);
    }
    /**
     * 指定的端口启动
     */
    void run(uint16_t port = 8000) {
        mep.set_reuse_addr(true);
        mep.listen(port);
        mep.start_accept();
        mep.run();
    }
    /**
     * 注册事件
     */
    void on(const std::string& en, efbk fbk) {
        if(en.length() && fbk)
            ee[en].push_back(fbk);
    }
    /**
     * 移除事件
     * 是移除事件的所有处理函数 暂时用不到
     */
    void remove(const std::string& en) {
        auto iter = ee.find(en);
        if (ee.cend() != iter)
            ee.erase(iter);
    }
    /**
     * 通过hdl获取到用户信息
     */
    HDLInfo* getInfoByHdl(HDL hdl) {
        auto iter = hdl2info.find(hdl);
        if (iter == hdl2info.cend()) {
            return NULL;
        }
        return iter->second;
    }
    /**
     * 通过uid获取到hdl
     * 可以用于向指定用户发送消息
     * 需要先判断.lock()是否为null
     */
    HDL getHDLByUid(int uid) const{
        static HDL hdl_null;
        auto iter = uid2hdl.find(uid);
        if (iter == uid2hdl.cend()) {
            return hdl_null;
        }
        return iter->second;
    }
    /**
     * 添加uid和hdl
     * 返回随机的认证码
     */
    int addHDLAndUid(int uid, HDL hdl) {
        int rid = rand();
        hdl2info[hdl] = new HDLInfo(uid, rid);
        uid2hdl[uid] = hdl;
        return rid;
    }
    /**
     * hdl信息移除掉
     * 未有相关信息 返回0
     */
    bool removeHDL(HDL hdl) {
        auto iter = hdl2info.find(hdl);
        if (iter != hdl2info.cend()) {
            roomManager.getOutRoom(iter->second->uid);  // 2 在移除时检查退出房间
            uid2hdl.erase(iter->second->uid);
            hdl2info.erase(iter);
            delete iter->second;
            return true;
        }
        return false;
    }
    /**
     * 更新hdl，在用户重连时使用
     * 使用rid作认证
     * 返回更新是否成功
     */
    bool updateHDL(HDL hdl, int uid, int rid) {
        if (getInfoByHdl(hdl)) return false;    // 正常状态不能重连
        // 找到重连信息
        WS::HDLInfo * info = NULL;
        for (auto i : rll) {
            if (i->uid == uid) {
                if (i->rid == rid) {
                    info = i;
                    break;
                }
            }
        }
        if (!info) return false;
        rll.erase(info);
        info->tmo = 0;
        hdl2info[hdl] = info;
        uid2hdl[uid] = hdl;
        return true;
    }
}ws;    // 与std::ws重名



#endif
