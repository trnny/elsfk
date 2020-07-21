#ifndef __MYSQLFUUU__
#define __MYSQLFUUU__


#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <mysql/mysql.h>

#define _PRMYSQLERR_  fprintf(stderr, "[MySQL ERROR] errno:%d error:%s\n", mysql_errno(conn), mysql_error(conn));

using std::vector;
using std::string;

class SimpleLock{
    bool islock = false;
    void wait() {
        while (islock);
        islock = true;
    }
    void done() {
        islock = false;
    }
};

SimpleLock lock;

class MyMysql {
    bool ok = false;
    MYSQL* conn = NULL;
    MYSQL_RES * res = NULL;
    MYSQL_ROW row = NULL;
public:
    MyMysql() {
        conn = mysql_init(NULL);
        if (NULL == conn || NULL == mysql_real_connect(conn, "127.0.0.1", "root", "0414tu..", "ELSFK", 3306, NULL, 0)) {
            _PRMYSQLERR_
            return;
        }
        ok = true;
    }
    ~MyMysql() {
        if (ok)
            mysql_close(conn);
    }
    /**
     * 查询
     * 返回查询执行是否成功
     * 需要获取查询的结果，使用result函数
     */
    bool query(const char* q) {
        if (!ok) return false;
        int r = mysql_query(conn, q);
        if (r == 0)
            return true;
        if (r == CR_SERVER_LOST || r == CR_SERVER_GONE_ERROR) {
            do {
                ok = false;
                printf("[MySQL] 重新连接...");
                conn = mysql_init(conn);
                if (NULL == conn || NULL == mysql_real_connect(conn, "127.0.0.1", "root", "0414tu..", "ELSFK", 3306, NULL, 0))
                    break;
                ok = true;
                printf("[MySQL] 重新连接成功");
                if(mysql_query(conn, q) == 0)
                    return true;
            }while (0);
        }
        _PRMYSQLERR_
        return false;
    }
    /**
     * 查询
     * 返回查询执行是否成功
     * 需要获取查询的结果，使用result函数
     */
    bool query(const string& q) {
        return query(q.c_str());
    }
    /**
     * 读取上次查询结果 
     * 读取成功则将结果放入二维字符串向量c中
     * 返回读取是否成功
     */
    bool result(vector<vector<string>>& c) {
        if (!ok) return false;
        res = mysql_store_result(conn);
        auto col = mysql_field_count(conn);
        if (NULL == res) {
            if (col == 0)
                _PRMYSQLERR_    // 有错误
            return false;       // 没有值也没有错误 例如插入 删除 更新
        }
        c.resize(mysql_num_rows(res));
        for (auto& ci : c) {
            ci.resize(col);
            row = mysql_fetch_row(res);
            for (auto& cj : ci) {
                cj = *row++;
            }
        }
        mysql_free_result(res);
        return true;
    }
    /**
     * 读取受影响行数(增删改)
     * 查询时返回查询结果行数
     * 若读取失败(MySQL断开)返回(uint64_t)-1
     */
    uint64_t affected_rows() {
        if (!ok) return -1;
        return mysql_affected_rows(conn);
    }
};


#endif