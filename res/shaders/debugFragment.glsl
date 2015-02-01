#version 330 core

// Ouput data
in vec2 textCoord;

out vec3 fragColor;

uniform sampler2D myTextureSampler;
void main()
{
   fragColor = texture(myTextureSampler, textCoord).rgb;
}
