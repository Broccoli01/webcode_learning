#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/Logging.h>
#include <functional>
#include <iostream>

using namespace std;
using namespace muduo;
using namespace muduo::net;

//----------------------------------------------------------------------
// ChatServer 类：封装了 TcpServer，并注册了连接/消息回调
//----------------------------------------------------------------------
class ChatServer
{
public:
    // 构造函数：传入 EventLoop*、监听端口
    // 事件循环+本地监听地址
    ChatServer(EventLoop *loop, const InetAddress &listenAddr)
        : loop_(loop), server_(loop, listenAddr, "ChatServer")
    {
        // 设置连接回调
        server_.setConnectionCallback(std::bind(&ChatServer::onConnection,
                                                this, std::placeholders::_1));
        // 设置消息回调
        server_.setMessageCallback(
            std::bind(&ChatServer::onMessage, this,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      std::placeholders::_3));
        // 可选：设置底层 subloop 数目（I/O 线程数）
        server_.setThreadNum(4);
    }
    // 启动服务器
    void start()
    {
        server_.start();
    }

private:
    // 新连接建立或断开时被调用
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            LOG_INFO << "New connection from "
                     << conn->peerAddress().toIpPort();
        }
        else
        {
            LOG_INFO << "Connection closed: "
                     << conn->peerAddress().toIpPort();
        }
    }
    // 收到数据时被调用
    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp receiveTime)
    {
        // 从缓冲区中读取全部可读数据
        string msg = buf->retrieveAllAsString();
        LOG_INFO << "Received at " << receiveTime.toString()
                 << " msg: " << msg;

        // 简单回显：将收到的消息原样返回
        conn->send(msg);
    }
    EventLoop *loop_;
    TcpServer server_;
};

/*
    基于muduo网络库开发服务器程序

    1.创建TcpServer对象
    2.创建EventLoop 事件循环对象 的 指针
    3.明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
    4.在服务器类里注册链接和断开事件回调函数和消息接收事件回调函数

*/
class ChatServer1
{
public:
    ChatServer1(EventLoop *loop,               // 事件循环
                const InetAddress &listenAddr, // IP+PORT
                const string &nameArg)         // 服务器名字
        : _loop(loop), _server(loop, listenAddr, nameArg)
    {
        // 给服务器注册用户连接的创建和断开回调
        _server.setConnectionCallback(std::bind(&ChatServer1::onConnection,
                                                this, std::placeholders::_1));
        // 给服务器注册用户读写事件回调
        _server.setMessageCallback(std::bind(&ChatServer1::onMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             std::placeholders::_3));
        //  设置服务器端的线程数量
        _server.setThreadNum(2);
    }

    void start()
    {
        _server.start();
    }

private:
    // 专门处理用户的连接创建和断开   epoll
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            cout << conn->peerAddress().toIpPort() << "->"
                 << conn->localAddress().toIpPort() << " State: online " << endl;
        }
        else
        {
            cout << conn->peerAddress().toIpPort() << "->"
                 << conn->localAddress().toIpPort() << " State: offline " << endl;
        }
    }
    void onMessage(const TcpConnectionPtr &conn, // 连接
                   Buffer *buf,                  // 缓冲区
                   Timestamp time)               // 接受到数据的时间信息
    {
        string buffer = buf->retrieveAllAsString();
        cout << "recv data: " << buffer << " time: " << time.toString() << endl;
        conn->send(buffer);
    }
    TcpServer _server; // #1
    EventLoop *_loop;  // #2 epoll
};

//----------------------------------------------------------------------
// main 函数：创建 EventLoop 和 ChatServer，然后进入循环
//----------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // // 支持命令行指定监听端口，默认 9981
    // uint16_t port = 9981;
    // if (argc > 1)
    // {
    //     port = static_cast<uint16_t>(atoi(argv[1]));
    // }

    // // 日志级别设置（可选）
    // muduo::Logger::setLogLevel(muduo::Logger::INFO);

    // // 创建事件循环
    // EventLoop loop;

    // // 本地监听地址
    // InetAddress listenAddr(port);

    // // 创建并启动服务器
    // ChatServer server(&loop, listenAddr);
    // server.start();

    // // 进入事件循环（阻塞）
    // loop.loop();
    // return 0;

    Logger::setLogLevel(Logger::INFO);
    EventLoop loop; // epoll

    InetAddress listenAddr("127.0.0.1", 6000);
    ChatServer1 server(&loop, listenAddr, "muduo_chat");

    cout << "Starting muduo chat server on port 6000..." << endl;

    server.start();
    loop.loop(); // epoll_wait以阻塞方式等待新用户链接，已连接用户的读写事件操作等

    return 0;
}