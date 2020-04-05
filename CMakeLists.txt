cmake_minimum_required (VERSION 3.17)

project (MyGE)

#VERSION
set (${PROJECT_NAME}_VERSION_MAJOR 0)
set (${PROJECT_NAME}_VERSION_MINOR 1)

# find_package(SDL2 REQUIRED)

#sub-systems
# add_subdirectory(src/systems)
# add_subdirectory(src/math)
add_subdirectory(src/core)
add_subdirectory(src/resource)
# add_subdirectory(src/debug)
#add_subdirectory(src/memory)
# add_subdirectory(src/events)
# add_subdirectory(src/render)

add_executable (MyGE src/main.cpp src/main.h)

target_compile_features(MyGE PUBLIC cxx_std_11)

# target_link_libraries(MyGE core SDL2::SDL2)

target_link_libraries(MyGE core resource)

if(UNIX)
	target_compile_options(MyGE PRIVATE -Werror -Wall -Wextra)
endif()

set_target_properties(MyGE
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/x86_64/"
)
