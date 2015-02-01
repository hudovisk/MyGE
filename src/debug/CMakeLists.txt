set(debug_source_files Logger.cpp DebugRenderManager.cpp)
set(debug_header_files Logger.h MyAssert.h DebugRenderManager.h)

add_library(my_debug ${debug_source_files} ${debug_header_files})

target_link_libraries(my_debug  render SDL2_ttf)

