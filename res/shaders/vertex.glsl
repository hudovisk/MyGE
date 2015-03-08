#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 texture_uv;

uniform mat4 model_Matrix;
uniform mat4 modelView_Matrix;
//uniform mat4 normal_Matrix;
uniform mat4 modelViewProjection_Matrix;
// uniform sampler2D gSampler; 

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vTexture;

void main()
{
    gl_Position = vertexPosition_modelspace * modelViewProjection_Matrix;
    // gl_Position = vertexPosition_modelspace;
    
    vWorldPos = (vertexPosition_modelspace * modelView_Matrix).xyz;
    //vPosition = vec3(gl_Position);
    vNormal = (vec4(normal_modelspace,0) * model_Matrix).xyz;

    vTexture = texture_uv.xy;
}
