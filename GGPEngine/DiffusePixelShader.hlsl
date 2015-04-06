#define DIRECTIONAL_LIGHT_COUNT 1
#define POINT_LIGHT_COUNT 1
#define SPOT_LIGHT_COUNT 1

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
	float	radius;
};

struct SpotLight
{
	float4	ambientColor;
	float4	diffuseColor;
	float3	direction;
	float	radius;
	float3	position;
	float	range;
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
	DirectionalLight	directionalLights[DIRECTIONAL_LIGHT_COUNT];
	PointLight			pointLights[POINT_LIGHT_COUNT];
	SpotLight			spotLights[SPOT_LIGHT_COUNT];
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

	for (int j = 0; j < POINT_LIGHT_COUNT; j++) {
		float3  pixelToLight = pointLights[j].position - pixel.positionT;
		float3	directionToLight = normalize(pixelToLight);
		float	contribution = saturate(dot(pixel.normal, directionToLight));
		float   attenuation = 1 - (length(pixelToLight) * (1 / pointLights[j].radius));
		colorAccumulator += ((contribution * pointLights[j].diffuseColor) + pointLights[j].ambientColor) * attenuation;
	}

	for (int k = 0; k < SPOT_LIGHT_COUNT; k++)
	{
		//float3  pixelToLight = spotLights[k].position - pixel.positionT;
		//float3	directionToLight = normalize(pixelToLight);
		//float	contribution = saturate(dot(pixel.normal, directionToLight));
		//float   linearAttenuation = lerp(1.0f, 0.0f, length(pixelToLight) / spotLights[k].range);

		//float   phi = radians(spotLights[k].radius);
		//float	theta = radians(spotLights[k].radius * 0.75f);
		//float	cosAlpha = max(0.0f, (dot(directionToLight, -normalize(spotLights[k].direction))));

		//float	totalAttenuation = 0.0f;
		//if (cosAlpha > theta) {
		//	totalAttenuation = 1.0f;
		//}
		//else if (cosAlpha > phi) { // Arbitrary Phi value
		//	totalAttenuation = pow((cosAlpha - phi) / (theta - phi), 32);
		//}

		//totalAttenuation *= linearAttenuation;
		//colorAccumulator += ((contribution * spotLights[k].diffuseColor) + spotLights[k].ambientColor) * totalAttenuation;
	}

	float4 textureColor = diffuseTexture.Sample(diffuseSampler, pixel.uv);
	return colorAccumulator * textureColor;
}


