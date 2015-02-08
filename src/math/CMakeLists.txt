
set(math_source_files Math.cpp Vec2.cpp ColorRGBA.cpp Vec3.cpp )
set(math_header_files Math.h Vec2.h ColorRGBA.h Vec3.h )

add_library(my_math ${math_source_files} ${math_header_files})
