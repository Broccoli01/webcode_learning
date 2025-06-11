#include "db/db.h"
#include <muduo/base/Logging.h> // muduo 日志库

// -----------------------------------------------------------------------------
// 配置数据库连接相关的静态参数（可根据实际环境修改）
// -----------------------------------------------------------------------------
static const std::string SERVER = "127.0.0.1"; // MySQL 服务器地址
static const std::string USER = "root";        // 登录用户名
static const std::string PASSWORD = "123456";  // 登录密码
static const std::string DBNAME = "chat";      // 数据库名称
static const unsigned PORT = 3306;             // 数据库端口号

// -----------------------------------------------------------------------------
// MySQL 构造函数：初始化 MYSQL 连接句柄
// -----------------------------------------------------------------------------
MySQL::MySQL()
{
    // mysql_init 会返回一个 MYSQL*，用于后续连接和操作
    _conn = mysql_init(nullptr);
}

// -----------------------------------------------------------------------------
// MySQL 析构函数：关闭连接并释放资源
// -----------------------------------------------------------------------------
MySQL::~MySQL()
{
    if (_conn)
    {
        // 关闭连接并释放内存
        mysql_close(_conn);
        _conn = nullptr;
    }
}

// -----------------------------------------------------------------------------
// 连接数据库
// 返回：true 表示连接成功，false 表示连接失败
// -----------------------------------------------------------------------------
bool MySQL::connect()
{
    // 调用 mysql_real_connect 建立连接
    // 参数：_conn、服务器地址、用户名、密码、数据库名、端口、unix_socket、客户端标志
    MYSQL *p = mysql_real_connect(
        _conn,
        SERVER.c_str(),
        USER.c_str(),
        PASSWORD.c_str(),
        DBNAME.c_str(),
        PORT,
        nullptr,
        0);

    if (p)
    {
        // 设置客户端与服务器交互的字符集为 GBK
        mysql_query(_conn, "SET NAMES gbk");
        LOG_INFO << __FILE__ << ":" << __LINE__
                 << " mysql连接成功: " << mysql_error(_conn);
        return true;
    }
    else
    {
        // 打印连接失败的日志，包含文件名、行号和错误描述
        LOG_INFO << __FILE__ << ":" << __LINE__
                 << " mysql连接失败: " << mysql_error(_conn);
        return false;
    }
}

// -----------------------------------------------------------------------------
// 执行增删改（INSERT/UPDATE/DELETE）操作
// 参数：sql 要执行的 SQL 语句
// 返回：true 表示执行成功，false 表示执行失败
// -----------------------------------------------------------------------------
bool MySQL::update(const std::string &sql)
{
    // 执行 SQL 语句
    if (mysql_query(_conn, sql.c_str()))
    {
        // 打印执行失败的日志，包含 SQL 语句及错误信息
        LOG_INFO << __FILE__ << ":" << __LINE__
                 << " [" << sql << "] 更新失败: "
                 << mysql_error(_conn);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// 执行查询（SELECT）操作
// 参数：sql 要执行的 SQL 查询语句
// 返回：MYSQL_RES* 查询结果集（失败返回 nullptr）
// -----------------------------------------------------------------------------
MYSQL_RES *MySQL::query(const std::string &sql)
{
    // 执行查询
    if (mysql_query(_conn, sql.c_str()))
    {
        // 打印执行失败的日志
        LOG_INFO << __FILE__ << ":" << __LINE__
                 << " [" << sql << "] 查询失败: "
                 << mysql_error(_conn);
        return nullptr;
    }

    // 获取并返回结果集
    return mysql_use_result(_conn);
}

MYSQL *MySQL::getconnection() const { return _conn; }