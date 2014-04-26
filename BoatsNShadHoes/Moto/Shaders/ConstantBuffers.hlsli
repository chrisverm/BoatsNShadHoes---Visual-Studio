#ifndef CONSTANT_BUFFERS_HLSLI
#define CONSTANT_BUFFERS_HLSLI

#include "Lighting.hlsli"

cbuffer perFrame : register( b0 )
{
	matrix view;
	matrix projection;
};

cbuffer perModel : register( b1 )
{
	matrix world;
};

cbuffer perScene : register( b2 )
{
	PointLight pntLights[NUM_PNT_LIGHTS];
};

#endif