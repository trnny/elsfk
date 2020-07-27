#ifndef __REDISFUUU__
#define __REDISFUUU__


/**
 * 该文件
 * 提供redis的访问接口
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include "../timer.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::list;
using std::map;
using std::set;

class MyRedis {
    redisContext* c = NULL;
    bool ok = false;
    redisReply* r = NULL;

    bool relink() {

    }

public:
    MyRedis() {
        c = redisConnect("127.0.0.1", 6379);
        if (c->err) {
            redisFree(c);
            cerr << "[REDIS ERROR]: 连接到redis失败! " << c->errstr << endl;
            return;
        }
        ok = true;
        setInterval([&]{
            
        }, 10*60*1000);
    }
    /**
     * string类型值存储
     */
    bool setString(const string& key, const string& value) {
        string cmd = "set " + key + " \"" + value + "\"";
        return cmd_OK_Free(cmd);
    }
    bool getString(const string& key, string& value) {
        r = (redisReply*)redisCommand(c, "get %s", key.c_str());
        if (r->type == REDIS_REPLY_NIL) {
            value = "";
            freeReplyObject(r);
            return true;
        } else if (r->type != REDIS_REPLY_STRING) {
            value = r->str;
            freeReplyObject(r);
            return true;
        }
        cerr << "[REDIS ERROR]: 读取失败" << endl;
        freeReplyObject(r);
        redisFree(c);
        return false;
    }
    bool setMap(const string& key, const map<string, string>& value) {
        if (value.empty()) return false;
        string cmd = "hmset " + key;
        for (const auto & i : value) {
            cmd += " " + i.first + " \"" + i.second + "\"";
        }
        return cmd_OK_Free(cmd);
    }
    bool getMap(const string& key, map<string, string>& value) {

    }
    bool getValue(const string& key, int& value) {

    }
    bool setValue(const string& key, int value) {
        string cmd = "set " + key + " " + std::to_string(value);
        return cmd_OK_Free(cmd);
    }
    bool cmd_OK_Free(const string& cmd) {
        r = (redisReply*)redisCommand(c, cmd.c_str());
        if (NULL == r) {
            cerr << "[REDIS ERROR]: 执行失败" << endl;
            redisFree(c);
            return false;
        }
        if (!(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str, "OK") == 0)) {
            cerr << "[REDIS ERROR]: 执行失败" << endl;
            freeReplyObject(r);
            redisFree(c);
            return false; 
        }
        // 执行成功
        freeReplyObject(r);
        return true;
    }
    
    bool setSet(const string& key, const list<string>& value) {

    }
    bool setList(const string& key, const vector<string>& value) {

    }
    bool getList(const string& key, vector<string>& value) {

    }
    bool getList(const string& key, list<string>& value) {

    }
    bool setSet(const string& key, const set<string>& value) {

    }
    bool setSet(const string& key, const vector<string>& value) {

    }
    bool getSet(const string& key, set<string>& value) {

    }
    bool getSet(const string& key, vector<string>& value) {

    }
};






#endif