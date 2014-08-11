cmake_minimum_required (VERSION 2.6)

project (MyGE)

#VERSION
set (${PROJECT_NAME}_VERSION_MAJOR 0)
set (${PROJECT_NAME}_VERSION_MINOR 1)

#include
include_directories (include/)

add_executable (MyGE src/main.cpp src/engine.cpp)
