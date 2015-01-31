set(core_source_files Engine.cpp System.cpp)
set(core_header_files Engine.h System.h)

add_library(core ${core_source_files} ${core_header_files})

target_link_libraries(core events render my_debug SDL2)