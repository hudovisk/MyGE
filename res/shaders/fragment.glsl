#version 330

in vec3 vWorldPos;
in vec3 vNormal;
in vec3 vTangent;
in vec2 vTexture;

layout (location = 0) out vec3 WorldPosOut;
layout (location = 1) out vec3 NormalOut;
layout (location = 2) out vec3 DiffuseOut;

uniform sampler2D gDiffuseSampler;
uniform sampler2D gNormalsSampler;

uniform vec3 gDiffColor;

uniform int useDiffSampler;
uniform int useNormalSampler;

out vec4 fragColor;

vec3 calcBumpNormal()
{
	vec3 normal = normalize(vNormal);
	vec3 tangent = normalize(vTangent);
	tangent = normalize((tangent - dot(tangent, normal) * normal));

	vec3 bitangent = cross(tangent, normal);
	vec3 bumpNormal = texture2D(gNormalsSampler, vTexture.xy).xyz;
	bumpNormal = 2.0 * bumpNormal - vec3(1.0, 1.0, 1.0);

	mat3 TBN = mat3(tangent, bitangent, normal);
	vec3 newNormal = TBN * bumpNormal;
	newNormal = normalize(newNormal);
	return newNormal;
}

void main()
{
	WorldPosOut = vWorldPos;

	if(useNormalSampler == 1)
		NormalOut = calcBumpNormal();
	else
		NormalOut = normalize(vNormal);

	if(useDiffSampler == 1)
  		DiffuseOut = texture2D(gDiffuseSampler, vTexture.xy).xyz;
  	else
  		DiffuseOut = gDiffColor;
} 