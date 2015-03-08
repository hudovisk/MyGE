#version 330

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexture;

uniform sampler2D gSampler;

layout (location = 0) out vec3 WorldPosOut;
layout (location = 1) out vec3 NormalOut;
layout (location = 2) out vec3 DiffuseOut;
layout (location = 3) out vec3 TextCoordOut;

out vec4 fragColor;

void main()
{
	WorldPosOut = vWorldPos;
	NormalOut = normalize(vNormal);
  	DiffuseOut = texture2D(gSampler, vTexture.xy).xyz;
  	TextCoordOut = vec3(vTexture,0);
} 