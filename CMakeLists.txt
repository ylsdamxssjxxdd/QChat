cmake_minimum_required(VERSION 3.12)
project(body)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Network Multimedia REQUIRED)
get_filename_component(Qt5_BIN_DIR "${Qt5_DIR}/../../../bin" ABSOLUTE)
message(STATUS "Qt5 bin:  ${Qt5_BIN_DIR}")
message(STATUS "build:  ${CMAKE_BINARY_DIR}")

add_subdirectory(src)
