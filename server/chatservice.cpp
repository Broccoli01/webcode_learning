#include "chatservice.hpp"      // 聊天业务逻辑类 ChatService 的声明
#include "public.hpp"           // 公共头文件，定义了消息 ID（如 LOGIN_MSG、REG_MSG 等）
#include <string>               // std::string
#include <muduo/base/Logging.h> // Muduo 日志库，用于 LOG_ERROR、LOG_INFO 等

// 单例模式：获取 ChatService 的唯一实例
ChatService *ChatService::instance()
{
    // 局部静态变量，第一次调用时构造，后续直接返回同一实例
    static ChatService service;
    return &service;
}

// 构造函数：注册所有支持的消息及其对应的处理回调
ChatService::ChatService()
{
    // 将登录消息 ID 映射到本类的 login 方法
    // std::bind 将成员函数和 this、以及三个占位符绑定成一个可调用对象
    _msgHandlerMap[LOGIN_MSG] =
        std::bind(&ChatService::login, this, _1, _2, _3);

    // 将注册消息 ID 映射到本类的 reg 方法
    _msgHandlerMap[REG_MSG] =
        std::bind(&ChatService::reg, this, _1, _2, _3);
}

// 根据 msgid 获取对应的处理回调
// 如果不存在，则返回一个打印错误的默认回调
MsgHandler ChatService::getHandler(int msgid)
{
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        // 未找到对应的 msgid，返回一个 lambda：
        // 捕获 msgid，接收连接、JSON、时间戳，打印错误日志
        return [=](const muduo::net::TcpConnectionPtr &conn,
                   nlohmann::json &js,
                   muduo::Timestamp)
        {
            LOG_ERROR << "msgid: " << msgid << " can not find handler!";
        };
    }
    else
    {
        // 找到则返回已注册的回调
        return it->second;
    }
}

// 处理登录请求的业务逻辑（stub）
//  conn - 客户端连接
//  js   - 客户端发送的 JSON 数据
//  time - 接收时间戳
void ChatService::login(const muduo::net::TcpConnectionPtr &conn,
                        nlohmann::json &js,
                        muduo::Timestamp time)
{
    // TODO: 实现登录验证、更新用户状态、返回结果给客户端等
    LOG_INFO << "do login service!!";
}

// 处理注册请求的业务逻辑（stub）
//  conn - 客户端连接
//  js   - 客户端发送的 JSON 数据
//  time - 接收时间戳
void ChatService::reg(const muduo::net::TcpConnectionPtr &conn,
                      nlohmann::json &js,
                      muduo::Timestamp time)
{
    // TODO: 实现用户注册、写库、返回注册结果给客户端等
    LOG_INFO << "do login service!!";
    string name = js["name"];
    string password = js["password"];

    User user;
    user.setName(name);
    user.setPassword(password);
    bool state = _userModel.insert(user);

    if(state){
        //注册成功
        nlohmann::json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else{
        //注册失败
        nlohmann::json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        conn->send(response.dump());
    }

}
