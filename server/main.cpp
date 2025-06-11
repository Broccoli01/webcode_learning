#include "chatserver.hpp"   // 自定义的 ChatServer 类声明，封装了服务器的核心逻辑
#include "chatserver.hpp"   // 自定义的 ChatServer 类声明，封装了服务器的核心逻辑
#include <iostream>         // 标准 I/O 流，用于输出日志或错误信息

int main() {
    // 1. 创建一个事件循环对象（Reactor 模式的核心），
    //    负责监听和分发网络事件
    EventLoop loop;

    // 2. 构造服务器监听地址：IP 为 127.0.0.1，端口为 6000
    InetAddress addr("127.0.0.1", 6000);

    // 3. 实例化聊天服务器
    //    参数依次为：
    //      &loop     - 指向事件循环的指针
    //      addr      - 监听地址（IP + 端口）
    //      "ChatServer" - 服务器名称（用于日志或统计）
    ChatServer server(&loop, addr, "ChatServer");

    // 4. 启动服务器，开始监听客户端连接
    server.start();
    std::cout << "ChatServer started, listening on 127.0.0.1:6000..." << std::endl;

    // 5. 启动事件循环，进入阻塞状态：
    //    当有网络事件（连接、读写等）发生时，
    //    会调用之前在 ChatServer 中绑定的回调函数
    loop.loop();

    // 程序正常退出
    return 0;
}
