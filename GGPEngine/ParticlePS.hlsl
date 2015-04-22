float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

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
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
};

//
// Pixel Shader
//
float4 ParticlePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float4 finalColor;

	return input.color;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

		// Combine the texture color and the particle color to get the final color result.
		finalColor = textureColor * input.color;

	return finalColor;
}