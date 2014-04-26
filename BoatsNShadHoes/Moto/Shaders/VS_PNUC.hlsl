#include "ConstantBuffers.hlsli"

// Defines what kind of data to expect as input
// - This should match our input layout!
struct VertexShaderInput
{
	float3 position	: POSITION0;
	float3 normal	: NORMAL0;
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR0;
};

// Defines the output data of our vertex shader
// - At a minimum, you'll need an SV_POSITION
// - Should match the pixel shader's input
struct VertexToPixel
{
	float4 position	: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 normal	: NORMAL0;
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR0;
	
	float3 worldPos : POSITION;
	PointLight pntLights[NUM_PNT_LIGHTS] : NEARESTLIGHT;
};

// The entry point for our vertex shader
VertexToPixel main( VertexShaderInput input )
{
	// Set up output
	VertexToPixel output;

	// Calculate output position
	matrix worldViewProj = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), worldViewProj);
	output.worldPos = mul(float4(input.position, 1.0f), world).xyz;

	output.uv	  = input.uv;
	output.color  = input.color;
	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal);

	output.pntLights = pntLights;

	return output;
}