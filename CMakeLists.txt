# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.0)

# 项目名称
project(nyx)

# 指定编译flag
set(CMAKE_CXX_FLAGS "-std=c++20")

# 设置宏
add_definitions(-D DEBUG)

# 指定include 的头文件的位置
include_directories(src/)

# 添加代码
#aux_source_directory(./test DIR_SRCS)

# 设置链接库路径
#link_directories(../jsoncpp-1.9.4/build/lib)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量

#add_executable(test ${DIR_SRCS})

file(GLOB SOURCE "test/*.cpp")
# 添加库
#target_link_libraries(test jsoncpp_static)
#message(${SOURCE})
foreach(f ${SOURCE})
    #message(${f})
    string(REPLACE ".cpp" "" testname ${f})
    #message(${testname})
    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/test/ "" name ${testname})
    #message(${name})
    add_executable(${name} ${f})
endforeach(f)

