TextureCube myTexture : register(t0);
SamplerState mySampler: register(s0);

// Defines the input to this pixel shader
// - Should match the corresponding vertex shader
struct VertexShaderOutput
{
	float4 position			: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	//float3 cameraPosition	: POSITION;
	float3 uv				: TEXCOORD0;
};

float4 main(VertexShaderOutput input) : SV_TARGET
{
	//return myTexture.Sample(mySampler, normalize(input.uv)); // re-normalize the interpolated unit vector
	return myTexture.Sample(mySampler, input.uv);
}