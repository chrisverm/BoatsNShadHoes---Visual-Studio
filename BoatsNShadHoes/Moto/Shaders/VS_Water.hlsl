#include "ConstantBuffers.hlsli"

// Defines what kind of data to expect as input
// - This should match our input layout!
struct VertexShaderInput
{
	float3 position	: POSITION0;
	float3 normal	: NORMAL0;
	float2 uv		: TEXCOORD0;
	float3 offset	: POSITION1;
};

// Defines the output data of our vertex shader
// - At a minimum, you'll need an SV_POSITION
// - Should match the pixel shader's input
struct VertexToPixel
{
	float4 position	: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD0;

	float3 worldPos : POSITION;
	PointLight pntLights[NUM_PNT_LIGHTS] : NEARESTLIGHT;
};

// The entry point for our vertex shader
VertexToPixel main( VertexShaderInput input )
{
	float timeAngle = time * 6.2831f;
	float distFromRest = 0.25f;

	float4 newPos = float4(input.position.x,
						   input.position.y + sin(timeAngle) * distFromRest,
						   input.position.z,
						   1.0f);

	// Set up output
	VertexToPixel output;

	// Calculate output position
	matrix worldViewProj = mul(mul(world, view), projection);
	output.position = mul(newPos, worldViewProj);
	output.worldPos = mul(newPos, world).xyz;

	output.uv	  = input.uv;
	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal);

	output.pntLights = pntLights;

	return output;
}