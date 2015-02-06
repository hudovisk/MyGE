cmake_minimum_required (VERSION 2.6)

project (MyGE)

set(CMAKE_CXX_FLAGS "-Wall -std=c++11")

#VERSION
set (${PROJECT_NAME}_VERSION_MAJOR 0)
set (${PROJECT_NAME}_VERSION_MINOR 1)

#include
include_directories(src/)
include_directories(extra/)

#sub-systems
add_subdirectory(src/core)
add_subdirectory(src/debug)
#add_subdirectory(src/memory)
add_subdirectory(src/events)
add_subdirectory(src/render)

add_executable (MyGE src/main.cpp)

target_link_libraries(MyGE core)
