set(core_source_files 
	Engine.cpp 
	Platform.cpp 
	EntityManager.cpp 
	ResourceManager.cpp)

set(core_header_files 
	Engine.h 
	Platform.h 
	EntityManager.h 
	Messages.h 
	ResourceManager.h )

add_library(core ${core_source_files} ${core_header_files})

target_link_libraries(core 
	events render my_systems my_debug 
	my_math ${EXTRA_LIBS_PATH}/SDL2.lib ${EXTRA_LIBS_PATH}/assimp.lib)