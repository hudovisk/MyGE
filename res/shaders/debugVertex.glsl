#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec2 texture_uv;

uniform mat4 modelView_Matrix;

out vec2 textCoord;

void main()
{
	vec3 vertexPosition_eyespace = (vertexPosition_modelspace * modelView_Matrix).xyz;
	gl_Position.xyz = vertexPosition_eyespace;
	
    //vColor = vec3(1,0,0);
    //vColor = vec3(1,1,1) * (difuse + 0.3);

    textCoord = texture_uv;

    //vColor = vertexPosition_modelspace.xyz;
    //vColor = normalPosition_modelspace.xyz;
}
