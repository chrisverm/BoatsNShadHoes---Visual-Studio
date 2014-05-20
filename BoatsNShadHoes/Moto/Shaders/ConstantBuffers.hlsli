#ifndef CONSTANT_BUFFERS_HLSLI
#define CONSTANT_BUFFERS_HLSLI

#include "Lighting.hlsli"

cbuffer perFrame : register( b0 )
{
	matrix view;
	matrix projection;

	float time;
	float3 cameraPosition;
};

cbuffer perModel : register( b1 )
{
	matrix world;
	float4 tint;
};

cbuffer perScene : register( b2 )
{
	PointLight pntLights[NUM_PNT_LIGHTS];
	float4 ambientLight;
};

#endif