set(render_source_files RenderManager.cpp Shader.cpp )
set(render_header_files RenderManager.h Shader.h Texture.h)

add_library(render ${render_source_files} ${render_header_files})

target_link_libraries(render my_debug SDL2 GL GLU GLEW)