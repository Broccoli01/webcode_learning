#include "Database.hpp"
#include <iostream>
#include <iomanip>

int main() {
    Database db;
    
    // 连接参数配置
    std::string host = "localhost";
    std::string user = "root";
    std::string password = "123456";  // 替换为你的密码
    std::string db_name = "cpp_demo";
    std::string socket = "/var/run/mysqld/mysqld.sock";
    
    // 连接数据库
    if (!db.connect(host, user, password, db_name, socket)) {
        std::cerr << "数据库连接失败，程序终止" << std::endl;
        return 1;
    }
    
    std::cout << "✅ 数据库连接成功！\n";
    
    // 创建示例表
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(50) NOT NULL,
            email VARCHAR(50) UNIQUE
        )
    )";
    
    if (db.executeUpdate(createTableSQL) >= 0) {
        std::cout << "✅ 用户表创建/验证成功\n";
    }
    
    // 插入数据（普通SQL）
    const char* insertSQL = "INSERT INTO users (name, email) VALUES "
                            "('Alice', 'alice@example.com'),"
                            "('Bob', 'bob@example.com')";
    
    int inserted = db.executeUpdate(insertSQL);
    if (inserted > 0) {
        std::cout << "✅ 插入 " << inserted << " 条记录成功\n";
    }
    
    // 插入数据（参数化查询 - 防SQL注入）
    auto stmt = db.prepareStatement("INSERT INTO users (name, email) VALUES (?, ?)");
    if (stmt) {
        stmt->setString(1, "Charlie");
        stmt->setString(2, "charlie@example.com");
        stmt->executeUpdate();
        std::cout << "✅ 参数化插入成功\n";
    }
    
    // 查询数据
    std::cout << "\n📊 用户列表:\n";
    std::cout << std::setw(5) << "ID" 
              << std::setw(15) << "Name" 
              << std::setw(25) << "Email" 
              << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    auto result = db.executeQuery("SELECT id, name, email FROM users");
    if (result) {
        while (result->next()) {
            std::cout << std::setw(5) << result->getInt("id")
                      << std::setw(15) << result->getString("name")
                      << std::setw(25) << result->getString("email")
                      << std::endl;
        }
    }
    
    // 清理资源
    db.disconnect();
    std::cout << "\n🔌 数据库连接已关闭\n";
    return 0;
}