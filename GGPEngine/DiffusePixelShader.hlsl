#define DIRECTONAL_LIGHT_COUNT 1
#define POINT_LIGHT_COUNT 1

struct DirectionalLight
{
	float4	ambientColor;
	float4	diffuseColor;
	float3	direction;
	float	padding;
};

struct PointLight
{
	float4	ambientColor;
	float4	diffuseColor;
	float3	position;
	float	padding;
};

struct Pixel
{
	float4	position	: SV_POSITION;
	float3	normal		: NORMAL;
	float3	positionT	: TEXCOORD;
	float2	uv			: TEXCOORD1;
};

SamplerState	diffuseSampler	: register(s0);
Texture2D		diffuseTexture	: register(t0);
cbuffer			lights			: register (b0)
{
	DirectionalLights[DIRECTIONAL_LIGHT_COUNT]	directionalLights;
	PointLights[POINT_LIGHT_COUNT]				pointLights;
}

float4 main(Pixel pixel) : SV_TARGET
{
	pixel.normal = normalize(pixel.normal);
	float4 colorAccumulator = float4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
		float3	directionToLight	=	normalize(-directionalLights[i].direction);
		float	contribution		=	saturate(dot(pixel.normal, directionToLight));
		colorAccumulator			+=	((contribution * directionalLights[i].diffuseColor) + directionalLights[i].ambientColor);
	}

	for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
		float3	directionToLight	=	normalize(pointLights[i].position - pixel.positionT);
		float	contribution		=	saturate(dot(pixel.normal, directionToLight));
		colorAccumulator			+=	((contribution * pointLights[i].diffuseColor) + pointLights[i].ambientColor);
	}

	float4 textureColor = diffuseTexture.Sample(diffuseSampler, pixel.uv);
	return colorAccumulator * textureColor;
}


