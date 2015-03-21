#version 330 core

struct DirLight
{
	vec3 direction;
	vec3 color;
	float ambIntensity;
	float diffIntensity;
};

uniform sampler2D gPositionMap;
uniform sampler2D gColorMap;
uniform sampler2D gNormalMap;

uniform vec2 gScreenSize;
uniform vec3 gEyeWorldPos;

uniform DirLight gLight;

out vec4 fragColor;

vec4 calcLightColor(vec3 position, vec3 normal, vec3 color, vec3 lightDir)
{
	vec4 ambientColor = vec4(gLight.color, 1.0) * gLight.ambIntensity;
	float diffuseFactor = dot(normal, -lightDir);

	vec4 diffuseColor = vec4(0, 0, 0, 0);
	vec4 specularColor = vec4(0, 0, 0, 0);

	if(diffuseFactor > 0)
	{
		diffuseColor = vec4(gLight.color, 1.0) * gLight.diffIntensity * diffuseFactor;

		// vec3 vertexToEye = normalize(gEyeWorldPos - position);
		// vec3 lightReflect = normalize(reflect(lightDir, normal));
		// float specularFactor = dot(vertexToEye, lightReflect);
		// if(specularFactor > 0)
		// {
		// 	specularFactor = pow(specularFactor, 50);
		// 	specularColor = vec4(gLight.color, 1.0) * specularFactor;	
		// }
	}

	return ambientColor + diffuseColor + specularColor;
}

void main()
{
	vec2 texCoord = gl_FragCoord.xy / gScreenSize;

	vec3 position = texture(gPositionMap, texCoord).xyz;
	vec3 color = texture(gColorMap, texCoord).xyz;
	vec3 normal = texture(gNormalMap, texCoord).xyz;
	normal = normalize(normal);

	vec3 lightDir = normalize(gLight.direction);

	vec4 lightColor = calcLightColor(position, normal, color, lightDir);

	fragColor = vec4(color, 1.0) * lightColor; 
}