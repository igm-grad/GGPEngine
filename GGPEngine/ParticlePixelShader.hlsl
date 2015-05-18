// Credit:
// http://www.rastertek.com/dx11tut39.html

/////////////
// GLOBALS //
/////////////
//Texture2D shaderTexture;
//SamplerState SampleType;

Texture2D		particleTexture	: register(t0);
SamplerState	omniSampler		: register(s0);

//////////////
// TYPEDEFS //
//////////////

/*struct PixelInputType
{
	float4	position	:SV_POSITION;
	float4	color		:COLOR;
	float3	size		:TEXCOORD0;
	float3	velocity	:TEXCOORD1;
	float	age			:TEXCOORD2;
	float2	uv			:TEXCOORD3;
};*/

struct GStoPS
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	//float2 uv			: TEXCOORD0;
};

/////////////////////////////
// Pixel Shader
/////////////////////////////
float4 main(GStoPS input) : SV_TARGET
{
	float4 textureColor;
	float4 finalColor;

	//return float4(0, 0, 0, 1);
	return input.color;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	//textureColor = particleTexture.Sample(omniSampler, input.uv);

	// Combine the texture color and the particle color to get the final color result.
	//finalColor = textureColor * input.color;

	return float4(1, 0, 0, 1);
}