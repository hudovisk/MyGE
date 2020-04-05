find_package(SDL2 REQUIRED)

set(core_source_files 
	Engine.cpp
	Platform.cpp
	debug/Logger.cpp
)

set(core_header_files 
	Engine.h
	debug/MyAssert.h
	Platform.h
	debug/Logger.h
)

add_library(core ${core_header_files} ${core_source_files})

target_compile_features(core PUBLIC cxx_std_11)

target_link_libraries(core PRIVATE SDL2::SDL2)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
