set(core_source_files 
	ResourceManager.cpp
)

set(core_header_files 
  ResourceManager.h
)

add_library(resource ${core_header_files} ${core_source_files})

target_link_libraries(resource PUBLIC core)

target_include_directories(resource PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})