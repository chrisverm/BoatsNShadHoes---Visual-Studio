#include "Lighting.hlsli"

Texture2D myTexture : register( t0 );
SamplerState mySampler : register( s0 );

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position	: SV_POSITION;
	float3 normal	: NORMAL0;
	float2 uv		: TEXCOORD0;

	float3 worldPos : POSITION;
	PointLight pntLights[NUM_PNT_LIGHTS] : NEARESTLIGHT;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 ambient = float4(0, 0, 0, 0);
	float4 diffuse = float4(0, 0, 0, 0);
	float4 spec	   = float4(0, 0, 0, 0);
	float4 A, D, S;

	DirectionalLight dirLight;
	dirLight.Diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	dirLight.Direction = normalize(float3(-1.0f, -1.0f, 1.0f));

	ComputeDirectionalLight(dirLight, input.normal, A, D, S);
	ambient += A;
	diffuse += D;
	spec	+= S;

	[unroll]
	for (int i = 0; i < NUM_PNT_LIGHTS; i++)
	{
		ComputePointLight(input.pntLights[i], input.worldPos, input.normal, A, D, S);
		ambient += A;
		diffuse += D;
		spec	+= S;
	}

	return myTexture.Sample(mySampler, input.uv) * (ambient + diffuse + spec);
}