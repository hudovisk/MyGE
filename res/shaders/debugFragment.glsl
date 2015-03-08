#version 330 core

// Ouput data
in vec2 textCoord;

out vec4 fragColor;

uniform sampler2D myTextureSampler;
void main()
{
   fragColor = texture(myTextureSampler, textCoord).rgba;
   // fragColor = vec4(255,0,0,255);
}
