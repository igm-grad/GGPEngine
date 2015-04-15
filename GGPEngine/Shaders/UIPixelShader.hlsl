struct Pixel
{
	float4	position	: SV_POSITION;
	float2 uv			: TEXCOORD;
};

SamplerState	uiSampler		: register(s0);
Texture2D		uiTexture		: register(t0);

float4 main(Pixel pixel) : SV_TARGET
{
	float4 textureColor = uiTexture.Sample(uiSampler, pixel.uv);

	return textureColor;
}


