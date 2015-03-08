#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec2 texture_uv;

uniform mat4 modelView_Matrix;

out vec2 textCoord;

void main()
{
	gl_Position = (vertexPosition_modelspace * modelView_Matrix);
	// gl_Position.xyz = vertexPosition_eyespace;
	
    textCoord = texture_uv;
}
