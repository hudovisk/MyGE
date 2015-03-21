#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 texture_uv;
layout(location = 3) in vec3 tangent_modelspace;

uniform mat4 World_Matrix;
uniform mat4 WVP_Matrix;

out vec3 vWorldPos;
out vec3 vNormal;
out vec3 vTangent;
out vec2 vTexture;

void main()
{
    gl_Position = vertexPosition_modelspace * WVP_Matrix;
    // gl_Position = vertexPosition_modelspace;
    
    vWorldPos = (vertexPosition_modelspace * World_Matrix).xyz;
    vNormal = (vec4(normal_modelspace,0) * World_Matrix).xyz;
    vTangent = (vec4(tangent_modelspace,0) * World_Matrix).xyz;

    vTexture = texture_uv.xy;
}
