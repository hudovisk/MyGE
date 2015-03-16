
#version 330

in vec3 vTexture;

uniform samplerCube gCubeMapSampler;

out vec4 fragColor;

void main()
{
  	fragColor = texture(gCubeMapSampler,  vTexture);
} 