#pragma once

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <string>
#include <memory>
#include <stdexcept>

/**
 * @class Database
 * @brief Linux平台专用的MySQL数据库操作封装类
 *
 * 核心优化点：
 * 1. 适配Linux环境下的库依赖和编译特性
 * 2. 增强连接参数配置（支持Unix socket连接）
 * 3. 内置UTF-8字符集配置
 */

class Database
{
public:
    Database();
    ~Database();
    /**
     * @brief 连接MySQL数据库（Linux特化版）
     * @param unix_socket 可选Unix socket路径（默认：/var/run/mysqld/mysqld.sock）
     * @param charset 字符集配置（默认：utf8mb4）
     */
    bool connect(const std::string &host = "localhost",
                 const std::string &user = "root",
                 const std::string &password = "123456",
                 const std::string &db_name = "cpp_demo",
                 const std::string &unix_socket = "/var/run/mysqld/mysqld.sock",
                 const std::string &charset = "utf8mb4");
    void disconnect();
    std::unique_ptr<sql::ResultSet> executeQuery(const std::string &query);                     // 执行查询
    int executeUpdate(const std::string &sql);                                        // 执行更新
    std::unique_ptr<sql::PreparedStatement> prepareStatement(const std::string &sql); // 参数化查询

private:
    sql::mysql::MySQL_Driver *driver;      // MySQL驱动实例
    std::unique_ptr<sql::Connection> conn; // 数据库连接对象
};