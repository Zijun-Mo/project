cmake_minimum_required(VERSION 3.10)

project(PROJECT)

# 指定C++版本
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用 -O2 优化选项
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 测试程序生成的路径
set(EXEC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 可执行文件
set(PROJECT minidb)
set(EXECUTABLE_OUTPUT_PATH ${EXEC_PATH})

# 添加头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加源文件
add_executable(${PROJECT} src/main.cpp src/parser.cpp src/calculation.cpp src/Global.cpp)