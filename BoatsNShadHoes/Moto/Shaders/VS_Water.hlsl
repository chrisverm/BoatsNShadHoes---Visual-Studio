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
	float3 normal	: NORMAL0;
	float2 uv		: TEXCOORD0;

	float3 worldPos : POSITION0;
	PointLight pntLights[NUM_PNT_LIGHTS] : NEARESTLIGHT;
};

float DegsToRads(float degrees)
{
	return degrees * (3.1415f / 180);
}

float3 WaveOffset(float3 inputPosition)
{
	float3 origin = mul(inputPosition, world).xyz;
	float start = (origin.x % 3.0f) * (3.1415f * 2);

	float timeAngle = (start + time) * 3.1415f;
	float amplitude = 0.5f;
	float frequency = 2.0f;

	float4 newPos = float4(cos(timeAngle * 2.0f) * 0.01f,
						   amplitude * sin(frequency * timeAngle),
						   0.0f,
						   0.0f);

	return newPos;
}

// The entry point for our vertex shader
VertexToPixel main( VertexShaderInput input )
{
	matrix worldViewProj = mul(mul(world, view), projection);

	// Set up output
	VertexToPixel output;

	// Calculate output position
	input.position += WaveOffset(input.position);

	output.position = mul(float4(input.position, 1.0f), worldViewProj);
	output.worldPos = mul(float4(input.position, 1.0f), world).xyz;

	output.uv	  = input.uv;
	output.uv.x -= time / 10.0f;

	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal);

	output.pntLights = pntLights;

	return output;
}