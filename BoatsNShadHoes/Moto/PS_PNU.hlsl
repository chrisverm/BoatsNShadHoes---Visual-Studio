
Texture2D myTexture : register( t0 );
SamplerState mySampler : register( s0 );

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position	: SV_POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD0;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float3 lightDirection = normalize(float3(0, 0, 1));
	float nDotL = saturate(dot(input.normal, -lightDirection));

	return myTexture.Sample(mySampler, input.uv) * nDotL;
}