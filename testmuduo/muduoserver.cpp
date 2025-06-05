#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/Logging.h>
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
    ChatServer(EventLoop *loop, const InetAddress &listenAddr) : loop_(loop), server_(loop, listenAddr, "ChatServer")
    {
        // 设置连接回调
        server_.setConnectionCallback(std::bind(&ChatServer::onConnection, this, std::placeholders::_1));
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



//----------------------------------------------------------------------
// main 函数：创建 EventLoop 和 ChatServer，然后进入循环
//----------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // 支持命令行指定监听端口，默认 9981
  uint16_t port = 9981;
  if (argc > 1)
  {
    port = static_cast<uint16_t>(atoi(argv[1]));
  }

  // 日志级别设置（可选）
  muduo::Logger::setLogLevel(muduo::Logger::INFO);

  // 创建事件循环
  EventLoop loop;

  // 本地监听地址
  InetAddress listenAddr(port);

  // 创建并启动服务器
  ChatServer server(&loop, listenAddr);
  server.start();

  // 进入事件循环（阻塞）
  loop.loop();
  return 0;
}