#include "ConstantBuffers.hlsli"

// Defines what kind of data to expect as input
// - This should match our input layout!
struct VertexShaderInput
{
	float3 position			: POSITION0;
};

// Defines the output data of our vertex shader
// - Should match the pixel shader's input
struct VertexShaderOutput
{
	float4 position			: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 uv				: TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
	// variables
	VertexShaderOutput output;
	matrix worldViewProj;

	worldViewProj	= mul(mul(world, view), projection); // screen space for an object
	
	output.position = mul(float4(input.position, 1.0f), worldViewProj).xyww; // screen space for the skybox, with z coords being 1.0 at all times (max depth)
	output.uv		= input.position; // texels for the UV are based off of world space coordinates

	return output;
}