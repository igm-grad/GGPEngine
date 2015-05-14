// Credit:
// http://www.rastertek.com/dx11tut39.html

/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

//////////////
// TYPEDEFS //
//////////////

struct PixelInputType
{
	//float4 position : SV_POSITION;
	
	//float4 color : COLOR;


	float4	position	:SV_POSITION;
	float4	color		:COLOR;
	float3	size		:TEXCOORD0;
	float3	velocity	:TEXCOORD1;
	float	age			:TEXCOORD2;
	float2	tex			:TEXCOORD3;
};

//
// Pixel Shader
//
float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float4 finalColor;

	//return float4(0, 0, 0, 1);
	return input.color;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	//textureColor = shaderTexture.Sample(SampleType, input.tex);

	// Combine the texture color and the particle color to get the final color result.
	//finalColor = textureColor * input.color;

	return float4(1, 0, 0, 1);
}