#include "ConstantBuffers.hlsli"

// Defines what kind of data to expect as input
// - This should match our input layout!
struct VertexShaderInput
{
	float3 position			: POSITION0;
	float3 cameraPosition	: POSITION1;
	float2 uv				: TEXCOORD0;	
};

// Defines the output data of our vertex shader
// - Should match the pixel shader's input
struct VertexShaderOutput
{
	float4 position			: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	//float3 cameraPosition	: POSITION;
	float3 uv				: TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
	// variables
	VertexShaderOutput output;
	float4 vertexPosition;
	matrix worldViewProj;
	
	// screen space
	worldViewProj = mul(mul(world, view), projection);
	
	// world space for uv's
	vertexPosition = mul(float4(input.position, 1.0f), world);

	// output values
	output.position			= mul(float4(input.position, 1.0f), worldViewProj);
	float4 cameraPosition	= mul(float4(input.cameraPosition, 1.0f), worldViewProj);
	output.uv				= vertexPosition - cameraPosition; // interpolates unit vector
	//output.uv				= vertexPosition - input.cameraPosition;
	//output.uv				= input.uv;

	return output;
}