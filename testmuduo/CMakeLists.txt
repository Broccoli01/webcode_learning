cmake_minimum_required(VERSION 3.5)
project(muduo_chat_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 允许用户通过 -DMUDUO_ROOT 指定 Muduo 安装路径（默认为 /usr/local）
# 如果不显式传入 MUDUO_ROOT，就默认用 ~/thirdparty/muduo
if (NOT DEFINED MUDUO_ROOT)
  # 注意：CMake 不会自动展开 "~"，用 $ENV{HOME} 代替
  set(MUDUO_ROOT "/home/xiaojy/thirdparty/muduo"
      CACHE PATH "Root directory of Muduo installation")
endif()

message(STATUS "Using Muduo from ${MUDUO_ROOT}")

# 包含头文件目录
include_directories(
  /usr/local/include
  ${MUDUO_ROOT}
)

# 设置链接目录
link_directories(/usr/local/lib)
# 添加可执行文件
add_executable(muduo_chat_server
    muduoserver.cpp
)
# 设置目标链接库
target_link_libraries(muduo_chat_server
    PRIVATE
    muduo_net
    muduo_base
    #jsoncpp
    ${CMAKE_THREAD_LIBS_INIT}  # pthread
)
