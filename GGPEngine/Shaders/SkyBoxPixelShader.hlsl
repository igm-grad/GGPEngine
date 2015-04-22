
// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 uv			: TEXCOORD;
};

Texture2D skyBoxTexture : register(t0);
SamplerState skyBoxSampler: register(s0);

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - Note that this color (like all values that pass through the rasterizer)
	//   is interpolated for each pixel between the corresponding 
	//   vertices of the triangle
	
	return	skyBoxTexture.Sample(skyBoxSampler, input.uv);
}