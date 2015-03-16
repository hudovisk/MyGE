#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;

// uniform mat4 model_Matrix;
uniform mat4 MVP_Matrix;
// uniform sampler2D gSampler; 

void main()
{
    gl_Position = vertexPosition_modelspace * MVP_Matrix;
}
