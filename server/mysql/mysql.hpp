#ifndef __MYSQLFUUU__
#define __MYSQLFUUU__

/**
 * 该文件
 * 提供MySQL的访问接口
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <mysql/mysql.h>

#define _PRMYSQLERR_  std::cerr << "[MySQL ERROR] errno:" << mysql_errno(conn) << " error:" << mysql_error(conn) << std::endl;


class SimpleLock{
    bool islock = false;
public:
    void wait() {
        while (islock);
        islock = true;
    }
    void done() {
        islock = false;
    }
}lock;

class Mysql {
    bool ok = false;
    MYSQL* conn = NULL;
    MYSQL_RES * res = NULL;
    MYSQL_ROW row = NULL;
public:
    Mysql() {
        conn = mysql_init(NULL);
        if (NULL == conn || NULL == mysql_real_connect(conn, "127.0.0.1", "root", "0414tu..", "ELSFK", 3306, NULL, 0)) {
            _PRMYSQLERR_
            return;
        }
        ok = true;
    }
    ~Mysql() {
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
            return true;    // 查询成功
        if (r == CR_SERVER_LOST || r == CR_SERVER_GONE_ERROR) {
            do {
                ok = false;
                std::cout << "[MySQL] 重新连接..." << std::endl;
                conn = mysql_init(conn);
                if (NULL == conn || NULL == mysql_real_connect(conn, "127.0.0.1", "root", "0414tu..", "ELSFK", 3306, NULL, 0))
                    break;
                ok = true;
                std::cout << "[MySQL] 重新连接成功" << std::endl;
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
     * 注意事项：    select语句后，必须读取结果，不然无法进行下一条语句
     *             可以重复读取，不会对后续操作影响
     *             select语句的结果不要使用affectd_rows获取，不能正常获取
     *             可以使用result保存到table里，根据table.size()获取行数
     */
    bool query(const std::string& q) {
        return query(q.c_str());
    }
    /**
     * 读取上次查询结果 
     * 读取成功则将结果放入二维字符串向量c中
     * 返回读取是否成功
     */
    bool result(std::vector<std::vector<std::string>>& c) {
        if (!ok) return false;
        res = mysql_store_result(conn);     // 结果0行也会成功
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
                if (*row)
                    cj = *row;
                ++row;
            }
        }
        mysql_free_result(res);
        return true;
    }
    /**
     * 读取受影响行数(增删改)
     * SELECT时返回查询结果行数  *******  与官方文档不一致 实际会返回(uint64_t)-1，并非结果行数
     * 若读取失败(MySQL断开)返回(uint64_t)-1
     */
    uint64_t affected_rows() {
        if (!ok) return -1;
        return mysql_affected_rows(conn);
    }
}sql;


#endif
