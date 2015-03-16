#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;

uniform mat4 MVP_Matrix;

out vec3 vTexture;

void main()
{
	vec4 position = vec4(vertexPosition_modelspace.xyz,0) * MVP_Matrix;
    
	gl_Position = position.xyww;

    vTexture = vertexPosition_modelspace.xyz;
    vTexture.z = -vTexture.z;
}