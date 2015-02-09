
set(math_source_files Quaternion.cpp Matrix4.cpp Matrix3.cpp Transform.cpp Vec3.cpp Vec4.cpp)
set(math_header_files Quaternion.h Matrix4.h Matrix3.h Transform.h Vec3.h Vec4.h)

add_library(my_math ${math_source_files} ${math_header_files})
