#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 texture_uv;
layout(location = 3) in vec3 tangent_modelspace;

uniform mat4 MVP_Matrix;

void main()
{
    gl_Position = vertexPosition_modelspace * MVP_Matrix;
}
