#include "ConstantBuffers.hlsli"

// Defines what kind of data to expect as input
// - This should match our input layout!
struct VertexShaderInput
{
	float3 position			: POSITION0;
	float2 uv				: TEXCOORD0;	
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
	float4 vertexPosition;
	float4 camPos;
	matrix worldViewProj;
	matrix worldView;
	matrix viewProj;
	matrix worldProj;
	
	worldViewProj	= mul(mul(world, view), projection); // screen space for an object
	worldView		= mul(world, view);
	viewProj		= mul(view, projection);
	worldProj		= mul(world, projection);
	vertexPosition	= mul(float4(input.position, 1.0f), world); // input position in world space
	camPos			= mul(float4(cameraPosition, 1.0f), worldViewProj);

	// output values
	output.position			= vertexPosition.xyww;
	//output.position		= mul(float4(input.position, 1.0f), worldViewProj).xyww;
	//output.uv				= normalize(vertexPosition - camPos); // interpolates unit vector
	//output.uv				= normalize(vertexPosition - cameraPosition);
	//output.uv				= input.position - cameraPosition;
	//output.uv				= vertexPosition - camPos;
	//output.uv				= vertexPosition - cameraPosition;
	//output.uv				= input.position - camPos;
	//output.uv				= normalize(input.position - cameraPosition);
	//output.uv				= normalize(input.position - camPos);
	//output.uv				= input.position;
	//output.uv				= normalize(mul(vertexPosition, worldViewProj));
	output.uv				= mul(vertexPosition, worldViewProj);
	//output.uv				= cameraPosition;
	//output.uv				= camPos;
	//output.uv				= normalize(mul(float4(input.position, 1.0f), worldViewProj));
	//output.uv				= mul(float4(input.position, 1.0f), worldViewProj);

	//output.uv = float3(1.0f, 0.5f, 0.3f);

	return output;
}