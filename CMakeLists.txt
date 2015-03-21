cmake_minimum_required (VERSION 2.6)

project (MyGE)

set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -g3")

#VERSION
set (${PROJECT_NAME}_VERSION_MAJOR 0)
set (${PROJECT_NAME}_VERSION_MINOR 1)

if(WIN32)
	set(EXTRA_LIBS_PATH ${CMAKE_SOURCE_DIR}/extra/lib/windows/x86_64)
else(UNIX)
	set(EXTRA_LIBS_PATH ${CMAKE_SOURCE_DIR}/extra/lib/linux/x86_64)
endif()

#libs
#link_directories(${CMAKE_SOURCE_DIR}/extra/lib/x64/)

#include
include_directories(src/)
include_directories(extra/include)

#sub-systems
add_subdirectory(src/systems)
add_subdirectory(src/math)
add_subdirectory(src/core)
add_subdirectory(src/debug)
#add_subdirectory(src/memory)
add_subdirectory(src/events)
add_subdirectory(src/render)

add_executable (MyGE src/main.cpp src/main.h)
if(WIN32)
	target_link_libraries(MyGE core ${EXTRA_LIBS_PATH}/SDL2main.lib)
else(UNIX)
	target_link_libraries(MyGE core)
endif()

set_target_properties( MyGE
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/x86_64/"
)
