
Texture2D myTexture : register( t0 );
SamplerState mySampler : register( s0 );

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position	: SV_POSITION;
	float3 normal	: NORMAL;
	float4 color	: COLOR;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float3 lightDirection = normalize(float3(0.0f, -1.0f, 1.0f));
	float nDotL = saturate(dot(input.normal, -lightDirection));

	return input.color * nDotL;
}