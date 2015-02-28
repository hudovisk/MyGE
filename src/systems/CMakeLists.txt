set(my_systems_source_files
 	MeshSystem.cpp 
 	SpatialSystem.cpp 
 	InputSystem.cpp 
 	MovementSystem.cpp 
 	PlayerControllerSystem.cpp
 	CameraSystem.cpp )

set(my_systems_header_files
 	MeshSystem.h 
 	SpatialSystem.h 
 	Component.h 
 	InputSystem.h 
 	MovementSystem.h 
 	PlayerControllerSystem.h
 	CameraSystem.h )

add_library(my_systems ${my_systems_source_files} ${my_systems_header_files})

target_link_libraries(my_systems core )