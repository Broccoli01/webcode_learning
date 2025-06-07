cmake_minimum_required(VERSION 3.10)
project(MySQL_Demo)

# 1. 查找MySQL开发包
find_library(MYSQL_CLIENT_LIB mysqlclient)
find_library(MYSQL_CPPCONN_LIB mysqlcppconn)  # 自动定位头文件和库路径[7,8](@ref)


#

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 3. 添加可执行文件
add_executable(
    MySQL_Demo
    src/main.cpp
    src/Database.cpp
)
target_include_directories(MySQL_Demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    /usr/include/mysql
)
add_definitions(-DLINUX_SOCKET_PATH=\"/var/run/mysqld/mysqld.sock\")

target_link_libraries(${PROJECT_NAME}
    #${MYSQL_CPPCONN_LIB}
    #${MYSQL_CLIENT_LIB}
    # ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MYSQL_CPPCONN_LIB}
    ${MYSQL_CLIENT_LIB}  # Linux必备链接库[1,5](@ref)
    pthread      # 多线程支持[1](@ref)
)
