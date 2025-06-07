#include "Database.hpp"
#include <stdexcept>
#include <iostream>

Database::Database() : driver(nullptr)
{
    // 初始化Mysql驱动（线程安全）
    driver = sql::mysql::get_mysql_driver_instance();
    if (!driver)
    {
        throw std::runtime_error("无法加载MySql驱动");
    }
}
Database::~Database()
{
    disconnect();
}

bool Database::connect(const std::string &host,
                       const std::string &user,
                       const std::string &password,
                       const std::string &db_name,
                       const std::string &unix_socket,
                       const std::string &charset)
{
    try
    {
        // 优先尝试Unix socket 链接（Linux优化）
        std::string url = "unix://" + unix_socket;
        conn.reset(driver->connect(url, user, password));

        // 选择数据库
        conn->setSchema(db_name);

        // 设置字符集
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        stmt->execute("SET NAMES '" + charset + "'");

        return true;
    }
    catch (const sql::SQLException &e)
    {
        // 如果Socket连接失败，尝试TCP连接
        try
        {
            std::string url = "tcp://" + host + ":3306";
            conn.reset(driver->connect(url, user, password));
            conn->setSchema(db_name);

            std::unique_ptr<sql::Statement> stmt(conn->createStatement());
            stmt->execute("SET NAMES '" + charset + "'");

            return true;
        }
        catch (const sql::SQLException &e)
        {
            std::cerr << "MySQL连接错误 #" << e.getErrorCode()
                      << ": " << e.what() << std::endl;
            return false;
        }
    }
}

void Database::disconnect()
{
    if (conn)
        conn->close();
}
std::unique_ptr<sql::ResultSet> Database::executeQuery(const std::string &query)
{
    try
    {
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        return std::unique_ptr<sql::ResultSet>(stmt->executeQuery(query));
    }
    catch (const sql::SQLException &e)
    {
        std::cerr << "查询错误 #" << e.getErrorCode()
                  << ": " << e.what() << std::endl;
        return nullptr;
    }
}
int Database::executeUpdate(const std::string &sql)
{
    try
    {
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        return stmt->executeUpdate(sql);
    }
    catch (const sql::SQLException &e)
    {
        std::cerr << "更新错误 #" << e.getErrorCode()
                  << ": " << e.what() << std::endl;
        return -1;
    }
}
std::unique_ptr<sql::PreparedStatement> Database::prepareStatement(const std::string &sql)
{
    try
    {
        return std::unique_ptr<sql::PreparedStatement>(conn->prepareStatement(sql));
    }
    catch (const sql::SQLException &e)
    {
        std::cerr << "预处理错误 #" << e.getErrorCode()
                  << ": " << e.what() << std::endl;
        return nullptr;
    }
}