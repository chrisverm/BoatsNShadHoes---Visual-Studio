#ifndef LIGHTING_HLSLI
#define LIGHTING_HLSLI

#define NUM_DIR_LIGHTS 0
#define NUM_PNT_LIGHTS 1
#define NUM_SPT_LIGHTS 0

struct Light
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
};

struct DirectionalLight : Light
{
	float3 Direction;
	float  padding;
};

struct PointLight : Light
{
	float  Range;
	float3 Position;
	float3 Attenuation;
	float  padding;
};

struct SpotLight : Light
{
	float  Range;
	float  Spot;
	float3 Position;
	float3 Direction;
	float3 Attenuation;
	float  padding;
};

void ComputeDirectionalLight(DirectionalLight light, float3 normal,
							 out float4 ambient, out float4 diffuse, out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.2f, 0.2f, 0.2f, 0.0f);
	spec	= float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightDirection = normalize(light.Direction);
	float nDotL = saturate(dot(normal, -lightDirection));

	diffuse += light.Diffuse * nDotL;
}

void ComputePointLight(PointLight light, float3 position, float3 normal,
					   out float4 ambient, out float4 diffuse, out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec	= float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = light.Position - position;
	float dist = length(lightVec);

	if (dist > light.Range)
		return;

	lightVec /= dist;

	float nDotL = saturate(dot(lightVec, normal));

	diffuse += light.Diffuse * nDotL;

	float attenuation = 1.0f / dot(light.Attenuation, float3(1.0f, dist, dist * dist));

	diffuse *= attenuation;
}

#endif