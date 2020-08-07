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

class Redis {
    redisContext* c = NULL;
    bool ok = true;
    redisReply* r = NULL;

public:
    Redis() {
        c = redisConnect("127.0.0.1", 6379);
        if (c->err) {
            cerr << "[REDIS ERROR]: 连接到redis失败! " << c->errstr << endl;
            redisFree(c);
            ok = false;
            return;
        }

        ok = simpleOKCmd("select 1");
    }
    ~Redis() {
        redisFree(c);
    }
    /**
     * string存储
     * 可以是二进制 
     */
    bool setString(const string& key, const string& value) {
        if (!ok) return false;
        r = (redisReply*)redisCommand(c, "set %s %b", key.c_str(), value.c_str(), value.size());
        if (NULL == r) {
            cerr << "[REDIS ERROR]: " << c->errstr << endl;
            ok = !redisReconnect(c);  // 重连
            return setString(key, value);
        }
        if (r->type == REDIS_REPLY_STATUS && strcasecmp(r->str, "OK") == 0) {
            freeReplyObject(r);
            return true;
        }
        cerr << "[REDIS SET ERROR]: " << r->str << endl;
        freeReplyObject(r);
        return false; 
    }
    /**
     * string追加
     * 可以是二进制
     */
    bool appendString(const string& key, const string& value) {
        if (!ok) return false;
        r = (redisReply*)redisCommand(c, "append %s %b", key.c_str(), value.c_str(), value.size());
        if (NULL == r) {
            cerr << "[REDIS ERROR]: " << c->errstr << endl;
            ok = !redisReconnect(c);  // 重连
            return appendString(key, value);
        }
        if (r->type == REDIS_REPLY_INTEGER) {
            freeReplyObject(r);
            return true;
        }
        cerr << "[REDIS APPEND ERROR]: " << r->str << endl;
        freeReplyObject(r);
        return false; 
    }
    /**
     * 读串
     * 只有读成功才返回true
     */
    bool getString(const string& key, string& value) {
        if (!ok) return false;
        r = (redisReply*)redisCommand(c, "get %s", key.c_str());
        if (NULL == r) {
            cerr << "[REDIS ERROR]: " << c->errstr << endl;
            ok = !redisReconnect(c);  // 重连
            return getString(key, value);
        }
        if (r->type == REDIS_REPLY_STRING) {
            value.resize(r->len);
            for (size_t i = 0; i < r->len; i++)
                value[i] = r->str[i];
            freeReplyObject(r);
            return true;
        }
        if (r->type == REDIS_REPLY_NIL) {
            freeReplyObject(r);
            return false;
        }
        cerr << "[REDIS GET ERROR]: " << r->str << endl;
        freeReplyObject(r);
        return false;
    }

    /**
     * 置表 失败不会回退已经设置的部分
     */
    bool setMap(const string& key, const map<string, string>& value) {
        if (!ok || value.empty()) return false;
        string cmd = "hmset " + key;
        bool skip = true;
        for (const auto & i : value) {
            if (i.second.find_first_of(" \t\r\n\0", 0, 5) != string::npos) {
                if (!setMapByField(key, i.first, i.second))
                    return false;
            }
            else{
                skip = false;
                cmd += " " + i.first + " " + i.second;
            }
        }
        return skip || simpleOKCmd(cmd);      // 对于不包含特殊字符的value 直接执行简单okcmd
    }
    /**
     * 置表某字段
     * 可以是二进制
     */
    bool setMapByField(const string& key, const string& field, const string& value) {
        if (!ok) return false;
        r = (redisReply*)redisCommand(c, "hset %s %s %b", key.c_str(), field.c_str(), value.c_str(), value.size());   // 3 1
        if (NULL == r) {
            cerr << "[REDIS ERROR]: " << c->errstr << endl;
            ok = !redisReconnect(c);  // 重连
            return setMapByField(key, field, value);
        }
        if (r->type == REDIS_REPLY_INTEGER) {
            freeReplyObject(r);
            return true;
        }
        cerr << "[REDIS HSET ERROR]: " << r->str << endl;
        freeReplyObject(r);
        return false; 
    }
    /**
     * 取整张map
     */
    bool getMap(const string& key, map<string, string>& value) {
        if (!ok) return false;
        r = (redisReply*)redisCommand(c, "hgetall %s", key.c_str());    // 2
        if (NULL == r) {
            cerr << "[REDIS ERROR]: " << c->errstr << endl;
            ok = !redisReconnect(c);  // 重连
            return getMap(key, value);
        }
        if (r->type == REDIS_REPLY_ARRAY) {
            value.clear();
            for (size_t i = 0; i < r->elements; ++i) {
                string& temp = value[r->element[i]->str];
                auto element_value = r->element[++i];
                temp.resize(element_value->len);
                for (size_t i = 0; i < element_value->len; i++)
                    temp[i] = element_value->str[i];
            }
            freeReplyObject(r);
            return true;
        }
        if (r->type == REDIS_REPLY_NIL) {
            freeReplyObject(r);
            return false;
        }
        cerr << "[REDIS ERROR]: " << r->str << endl;
        freeReplyObject(r);
        return false;
    }
    /**
     * 获取部分键的值
     * 表中不存在的字段值为空字符串
     */
    bool getMapByFields(const string& key, map<string, string>& value) {
        if (!ok || value.empty()) return false;
        string cmd = "hmget " + key;
        for (const auto& i : value) {
            cmd += " " + i.first;
        }
        r = (redisReply*)redisCommand(c, cmd.c_str());  // 5 ok
        if (NULL == r) {
            cerr << "[REDIS ERROR]: " << c->errstr << endl;
            ok = !redisReconnect(c);  // 重连
            return getMapByFields(key, value);
        }
        if (r->type == REDIS_REPLY_ARRAY) {
            int i = 0;
            for (auto& iter : value) {
                auto element_value = r->element[i++];
                if (element_value->type == REDIS_REPLY_STRING) {
                    iter.second.resize(element_value->len);
                    for (size_t i = 0; i < element_value->len; i++)
                        iter.second[i] = element_value->str[i];
                }
                if (element_value->type == REDIS_REPLY_NIL) {
                    iter.second = "";
                }
            }
            freeReplyObject(r);
            return true;
        }
        if (r->type == REDIS_REPLY_NIL) {
            freeReplyObject(r);
            return false;
        }
        cerr << "[REDIS ERROR]: " << r->str << endl;
        freeReplyObject(r);
        return false;
    }
    /**
     * 读表某个字段的值
     */
    bool getMapValueByField(const string& key, const string& field, string& value) {
        if (!ok) return false;
        r = (redisReply*)redisCommand(c, "hget %s %s", key.c_str(), field.c_str());
        if (NULL == r) {
            cerr << "[REDIS ERROR]: " << c->errstr << endl;
            ok = !redisReconnect(c);  // 重连
            return getMapValueByField(key, field,value);
        }
        if (r->type == REDIS_REPLY_STRING) {
            value.resize(r->len);
            for (size_t i = 0; i < r->len; i++)
                value[i] = r->str[i];
            freeReplyObject(r);
            return true;
        }
        if (r->type == REDIS_REPLY_NIL) {
            freeReplyObject(r);
            return false;
        }
        cerr << "[REDIS ERROR]: " << r->str << endl;
        freeReplyObject(r);
        return false;
    }
    /**
     * 读取int
     */
    bool getValue(const string& key, int& value) {
        if (!ok) return false;
        r = (redisReply*)redisCommand(c, "get %s", key.c_str());
        if (r->type == REDIS_REPLY_NIL) {
            freeReplyObject(r);
            return false;
        }
        if (r->type == REDIS_REPLY_STRING) {
            value = atoi(r->str);
            freeReplyObject(r);
            return true;
        }
        cerr << "[REDIS ERROR]: 读取失败" << endl;
        freeReplyObject(r);
        return false;
    }
    /**
     * 存int
     */
    bool setValue(const string& key, int value) {
        if (!ok) return false;
        string cmd = "set " + key + " " + std::to_string(value);
        return simpleOKCmd(cmd);
    }
    /** 
     * 执行简单返回为OK的指令
     */
    bool simpleOKCmd(const string& cmd) {
        if (!ok) return false;
        r = (redisReply*)redisCommand(c, cmd.c_str());
        if (NULL == r) {
            cerr << "[REDIS ERROR]: " << c->errstr << endl;
            ok = !redisReconnect(c);  // 重连
            return simpleOKCmd(cmd);
        }
        if (r->type == REDIS_REPLY_STATUS && strcasecmp(r->str, "OK") == 0) {
            freeReplyObject(r);
            return true; 
        }
        cerr << "[REDIS CMD ERROR]: " << r->str << endl;
        freeReplyObject(r);
        return false;
    }

    /**
     * 简单的指令  不能判断成功  不能获得结果
     */
    void simpleCmd(const string& cmd) {
        if (!ok) return;
        r = (redisReply*)redisCommand(c, cmd.c_str());
        if (NULL == r) {
            cerr << "[REDIS ERROR]: " << c->errstr << endl;
            ok = !redisReconnect(c);  // 重连
            simpleCmd(cmd);
        }
        // 不对r进行任何判断
        freeReplyObject(r);
    }
}redis;






#endif