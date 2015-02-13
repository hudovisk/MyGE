set(my_systems_source_files MeshSystem.cpp SpatialSystem.cpp PlayerInputSystem.cpp MovementSystem.cpp)
set(my_systems_header_files MeshSystem.h SpatialSystem.h Component.h PlayerInputSystem.h MovementSystem.h)

add_library(my_systems ${my_systems_source_files} ${my_systems_header_files})

target_link_libraries(my_systems core )