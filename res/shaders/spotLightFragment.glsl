#version 330 core

struct Attenuation
{
	float cte;
	float lin;
	float expo;
};

struct PointLight
{
	vec3 pos;
	vec3 direction;
	float minSpotFactor;
	vec3 color;
	float ambIntensity;
	float diffIntensity;
	Attenuation att;
};

uniform sampler2D gPositionMap;
uniform sampler2D gColorMap;
uniform sampler2D gNormalMap;
uniform sampler2D gShadowMap;

uniform vec2 gScreenSize;
uniform vec3 gEyeWorldPos;

uniform PointLight gLight;

uniform mat4 WVPLight_Matrix;

out vec4 fragColor;

float calcShadowFactor(vec3 position)
{

	vec4 shadowCoord = vec4(position, 1.0) * WVPLight_Matrix;
	shadowCoord.xyz /= shadowCoord.w;
	shadowCoord.x = shadowCoord.x*0.5 + 0.5;
	shadowCoord.y = shadowCoord.y*0.5 + 0.5;
	shadowCoord.z = shadowCoord.z*0.5 + 0.5;
	float shadowDepth = texture(gShadowMap, shadowCoord.xy).r;
	if(shadowDepth < (shadowCoord.z - 0.00001))
		return 0.1; //in shadow
	else
		return 1.0;
}

vec4 calcLightColor(vec3 position, vec3 normal, vec3 color, vec3 lightDir)
{
	vec4 ambientColor = vec4(gLight.color, 1.0) * gLight.ambIntensity;
	float diffuseFactor = dot(normal, -lightDir);
	
	float shadowFactor = calcShadowFactor(position);

	vec4 diffuseColor = vec4(0, 0, 0, 0);
	vec4 specularColor = vec4(0, 0, 0, 0);

	if(diffuseFactor > 0)
	{
		diffuseColor = vec4(gLight.color, 1.0) * gLight.diffIntensity * diffuseFactor;

		vec3 vertexToEye = normalize(gEyeWorldPos - position);
		vec3 lightReflect = normalize(reflect(lightDir, normal));
		float specularFactor = dot(vertexToEye, lightReflect);
		if(specularFactor > 0)
		{
			// specularFactor = pow(specularFactor, 3);
			specularColor = vec4(gLight.color, 1.0) * specularFactor;	
		}
	}

	return ambientColor + shadowFactor * (diffuseColor + specularColor);
}

void main()
{
	vec2 texCoord = gl_FragCoord.xy / gScreenSize;

	vec3 position = texture(gPositionMap, texCoord).xyz;
	vec3 color = texture(gColorMap, texCoord).xyz;
	vec3 normal = texture(gNormalMap, texCoord).xyz;
	normal = normalize(normal);

	vec3 lightDir = position - gLight.pos;
	float dist = length(lightDir);
	lightDir /= dist;

	float spotFactor = dot(gLight.direction, lightDir);
	if(spotFactor > gLight.minSpotFactor)
	{
		spotFactor = 1.0 - (1.0 - spotFactor) * 1/(1.0 - gLight.minSpotFactor);
		float attenuation = 1 +	gLight.att.expo * dist * dist;
		vec4 lightColor = calcLightColor(position, normal, color, lightDir);

		fragColor = vec4(color, 1.0) * lightColor * spotFactor / attenuation; 
	}
	else
	{
		fragColor = vec4(0, 0, 0, 1);
	}
	// float depth = pow(texture(gShadowMap, texCoord).r,50);
	// vec3 depthColor = vec3(depth, depth, depth);//texture(gShadowMap, texCoord).r;
	// fragColor = vec4(depthColor,1.0);
}