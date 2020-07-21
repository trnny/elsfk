#ifndef __WSSERVER__
#define __WSSERVER__


#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "msg.pb.h"

using namespace std;

typedef websocketpp::server<websocketpp::config::asio> server;
typedef google::protobuf::Map<string, Msg_VType> pb_map;
typedef google::protobuf::MapPair<string, Msg_VType> mp;
typedef google::protobuf::Map<string, Msg_VType>::const_iterator CITER;
typedef google::protobuf::Map<string, Msg_VType>::iterator ITER;
typedef websocketpp::connection_hdl HDL;

class WS
{
public:
    struct HDLInfo
    {
        int uid;    // 用户id
        int rid;    // 用户随机标识  用于重连认证
        int tmo;    // 超时标记  用于移除重连等待
    };
private:
    typedef function<void(HDL, const pb_map&)> efbk;
    map<string, vector<efbk>> ee;
    server mep;
    void emit(const string& en, HDL t1, const pb_map& t2) {
        auto iter = ee.find(en);
        if (iter != ee.cend())
            for (auto ef : iter->second)
                ef(t1, t2);
    }
    struct cmp_key
    {
        bool operator()(HDL hdl1, HDL hdl2) const
        {
            if (hdl1.lock() != hdl2.lock())
                return hdl1.lock() < hdl2.lock();
            return false;
        }
    };
    map<HDL, HDLInfo*, cmp_key> hdl2info;
    map<int, HDL> uid2hdl;
    vector<HDLInfo*> rll;   //  重连队列  超时、重连时移除
    // TO-DO  实现定时器，定时检查重连队列
public:
    WS() {
        mep.set_error_channels(websocketpp::log::elevel::all);
        mep.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);
        mep.init_asio();
        mep.set_message_handler([this](HDL hdl, server::message_ptr msg_ptr) {
            if(msg_ptr->get_opcode() == websocketpp::frame::opcode::binary) {
                Msg msg;
                if (msg.ParseFromString(msg_ptr->get_payload())) {
                    emit(msg.desc(), hdl, msg.data());
                }
            }
        });
        mep.set_close_handler([this](HDL hdl) {
            // 用户断开连接  断开连接时：等待重连队列、移除
        });
    }
    /**
     * 发送二进制消息
     * 目前，应该是发送pb序列化结果
     */
    void send(HDL hdl, const string& s) {
        mep.send(hdl, s, websocketpp::frame::opcode::binary);
    }
    /**
     * 指定的端口启动
     */
    void run(uint16_t port = 8000) {
        mep.listen(port);
        mep.start_accept();
        mep.run();
    }
    /**
     * 注册事件
     */
    void on(const string& en, efbk fbk) {
        if(en.length() && fbk)
            ee[en].push_back(fbk);
    }
    /**
     * 移除事件
     * 是移除事件的所有处理函数 暂时用不到
     */
    void remove(const string& en) {
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
    HDL getHDLByUid(int uid) {
        HDL hdl_null;   // .lock()是个null
        auto iter = uid2hdl.find(uid);
        if (iter == uid2hdl.cend()) {
            return hdl_null;
        }
        return iter->second;
    }
};



#endif