#define DIRECTIONAL_LIGHT_COUNT 1
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
	float4 position	: SV_POSITION;
	float3 normal	: NORMAL;
	float3 positionT: TEXCOORD;
	float3 tangent	: TEXCOORD1;
	float2 uv		: TEXCOORD2;
};

SamplerState	omniSampler		: register(s0);
Texture2D		diffuseTexture	: register(t0);
Texture2D		normalTexture	: register(t1);
cbuffer			lights			: register (b0)
{
	DirectionalLight	directionalLights[DIRECTIONAL_LIGHT_COUNT];
	PointLight			pointLights[POINT_LIGHT_COUNT];
}

float4 main(Pixel pixel) : SV_TARGET
{
	pixel.normal = normalize(pixel.normal);
	pixel.tangent = normalize(pixel.tangent);

	float3 binormal = cross(pixel.tangent, pixel.normal);
	float3x3 TBN = 
	{	
		pixel.tangent.x, pixel.tangent.y, pixel.tangent.z,
		binormal.x, binormal.y, binormal.z,
		pixel.normal.x, pixel.normal.y, pixel.normal.z 
	};

	float3 normal = normalTexture.Sample(omniSampler, pixel.uv).xyz;
	normal = (normal * 2.0f) - 1.0f;
	normal = mul(TBN, normal);

	float4 colorAccumulator = float4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
		float3	directionToLight = normalize(-directionalLights[i].direction);
		float	contribution = saturate(dot(normal, directionToLight));
		colorAccumulator += ((contribution * directionalLights[i].diffuseColor) + directionalLights[i].ambientColor);
	}

	for (int j = 0; j < POINT_LIGHT_COUNT; j++) {
		float3	directionToLight = normalize(pointLights[j].position - pixel.positionT);
		float	contribution = saturate(dot(normal, directionToLight));
		colorAccumulator += ((contribution * pointLights[j].diffuseColor) + pointLights[j].ambientColor);
	}

	float4 textureColor = diffuseTexture.Sample(omniSampler, pixel.uv);
	return colorAccumulator * textureColor;
}