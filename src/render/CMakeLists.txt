set(render_source_files 
	RenderManager.cpp 
	Shader.cpp
	GeometricPassShader.cpp
	LightPassShader.cpp )

set(render_header_files 
	RenderManager.h 
	Shader.h 
	GeometricPassShader.h
	LightPassShader.h
	Texture.h 
	Geometric.h)

add_library(render ${render_source_files} ${render_header_files})

IF(WIN32)
	set(OPENGL_LIBS OpenGL32 ${EXTRA_LIBS_PATH}/glew32.lib)
ELSE(UNIX)
	set(OPENGL_LIBS GL GLU GLEW)	
ENDIF()

target_link_libraries(render 
	core my_debug ${EXTRA_LIBS_PATH}/SDL2.lib
	 ${EXTRA_LIBS_PATH}/SDL2_image.lib ${OPENGL_LIBS})