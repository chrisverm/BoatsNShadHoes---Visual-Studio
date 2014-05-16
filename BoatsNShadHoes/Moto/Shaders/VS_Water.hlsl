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
	return degrees * (3.1415f / 180.0f);
}

float Mod(float num, float mod)
{
	return num - mod * floor(num / mod);
}

float3 WaveOffset(float3 inputPosition)
{
	float3 origin = mul(inputPosition, world).xyz;
	float start = (origin.x / 150.0f) * 360;

	float angle = (start + time * 200.0f) * 3.1415f;
	float amplitude = 2.0f;
	float frequency = 0.25f;
	float xShift = 0.025f;

	angle *= frequency;

	float waveX = cos(DegsToRads(angle)) * xShift;
	float waveY = sin(DegsToRads(angle)) * amplitude;

	return float3(waveX, waveY, inputPosition.z);
}

float3 WaveNormal(float3 inputPosition, float3 waveOffset)
{
	float3 origin = mul(inputPosition, world).xyz;
	float start = (origin.x / 150.0f) * 360;

	float angle = (start + time * 200.0f) * 3.1415f;
	float amplitude = 2.0f;
	float frequency = 0.25f;
	float xShift = 0.025f;

	angle *= frequency;

	angle = Mod(angle, 360);

	float diff = 1.0f;

	float2 frontPos = float2(origin.x, origin.y);
	frontPos.x = +20;
	frontPos.y += sin(DegsToRads(angle + diff)) * amplitude;

	float2 backPos = float2(origin.x, origin.y);
	backPos.x = -20;
	backPos.y += sin(DegsToRads(angle - diff)) * amplitude;

	float2 norm = frontPos - backPos;
	float3 retNorm = float3(-norm.y, norm.x, 0);

	return float3(-norm.y, norm.x, 0);
}

// The entry point for our vertex shader
VertexToPixel main( VertexShaderInput input )
{
	matrix worldViewProj = mul(mul(world, view), projection);

	// Set up output
	VertexToPixel output;

	// Calculate output position
	float4 position = float4(input.position + WaveOffset(input.position), 1.0f);

	output.position = mul(position, worldViewProj);
	output.worldPos = mul(position, world).xyz;

	output.uv	  = input.uv;
	output.uv.x -= time / 8.0f;

	float3 normal = WaveNormal(input.position, position);

	output.normal = mul(normal, (float3x3)world);
	output.normal = normalize(output.normal);

	output.pntLights = pntLights;

	return output;
}