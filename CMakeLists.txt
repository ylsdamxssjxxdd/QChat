cmake_minimum_required(VERSION 3.12)
project(body)
# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# msvc设置编译选项
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8") # 支持代码中的中文
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /utf-8") # 支持代码中的中文
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8") # 支持代码中的中文
# mingw设置编译选项
elseif(MINGW)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -Wall -Wextra -ffunction-sections -fdata-sections -fexceptions -mthreads") # 编译优化
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections -s") # 链接优化，减少体积
# linux下编译选项
elseif(UNIX)
    message(STATUS "Compiling on Unix/Linux")
endif()



find_package(Qt5 COMPONENTS Widgets Network Multimedia REQUIRED)
get_filename_component(Qt5_BIN_DIR "${Qt5_DIR}/../../../bin" ABSOLUTE)
message(STATUS "Qt5 bin:  ${Qt5_BIN_DIR}")
message(STATUS "build:  ${CMAKE_BINARY_DIR}")

# 添加源代码
add_subdirectory(src)
