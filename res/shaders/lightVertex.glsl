#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 texture_uv;

// uniform mat4 model_Matrix;
uniform mat4 modelViewProjection_Matrix;
// uniform sampler2D gSampler; 

void main()
{
    gl_Position = vertexPosition_modelspace * modelViewProjection_Matrix;
}
