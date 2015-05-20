// Credit:
// http://www.rastertek.com/dx11tut39.html

/////////////
// GLOBALS //
/////////////

Texture2D		particleTexture	: register(t0);
SamplerState	omniSampler	: register(s0);

//////////////
// TYPEDEFS //
//////////////

struct GStoPS
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
};

/////////////////////////////
// Pixel Shader
/////////////////////////////
float4 main(GStoPS input) : SV_TARGET
{
	float4 textureColor;
	float4 finalColor;

	//return input.color;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = particleTexture.Sample(omniSampler, input.uv);

	// Combine the texture color and the particle color to get the final color result.
	finalColor = textureColor * input.color;
	//finalColor = (input.uv.x, input.uv.y, 1, .5f);

	return finalColor;
}