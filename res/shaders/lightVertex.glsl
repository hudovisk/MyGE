#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;

uniform mat4 WVP_Matrix;

void main()
{
    gl_Position = vertexPosition_modelspace * WVP_Matrix;
}
