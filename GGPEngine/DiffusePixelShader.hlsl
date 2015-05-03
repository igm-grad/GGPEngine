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
	float4 position	: SV_POSITION;
	float3 normal	: NORMAL;
	float3 positionT: POSITIONT;
	float2 uv		: TEXCOORD;
};

SamplerState	diffuseSampler	: register(s0);
Texture2D		diffuseTexture	: register(t0);
cbuffer			lights			: register (b0)
{
	DirectionalLight	directionalLights[DIRECTIONAL_LIGHT_COUNT];
	PointLight			pointLights[POINT_LIGHT_COUNT];
	SpotLight			spotLights[SPOT_LIGHT_COUNT];
	float3				eyePosition;
	float				specularExponent;
}

float4 diffuseColor(float4 diffuseColor, float4 ambientColor, float3 pixelNormal, float3 directionToLight)
{
	directionToLight = normalize(directionToLight);
	float	contribution = saturate(dot(pixelNormal, directionToLight));
	return ((contribution * diffuseColor) + ambientColor);
}

float4 specularColor(float3 eyePosition, float3 pixelPositionT, float3 pixelNormal, float3 directionToLight)
{
	float3 pixelToEye = normalize(eyePosition - pixelPositionT);
	float3 pixelReflection = normalize(reflect(directionToLight, pixelNormal));
	float specularContribution = saturate(dot(pixelToEye, pixelReflection));
	return float4(1.f, 1.f, 1.f, 1.0f) * pow(specularContribution, specularExponent);
}

float4 main(Pixel pixel) : SV_TARGET
{
	pixel.normal = normalize(pixel.normal);
	float4 dColorAccumulator = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 aColorAccumulator = float4(0.0f, 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
		float4 dColor = diffuseColor(directionalLights[i].diffuseColor, directionalLights[i].ambientColor, pixel.normal, -directionalLights[i].direction);
		float4 sColor = specularColor(eyePosition, pixel.positionT, pixel.normal, directionalLights[i].direction);
		dColorAccumulator += (dColor + sColor);
		aColorAccumulator += directionalLights[i].ambientColor;
	}

	for (int j = 0; j < POINT_LIGHT_COUNT; j++) {
		float3  pixelToLight = pointLights[j].position - pixel.positionT;
		float4	dColor = diffuseColor(pointLights[j].diffuseColor, pointLights[j].ambientColor, pixel.normal, pixelToLight);
		float4  sColor = specularColor(eyePosition, pixel.positionT, pixel.normal, -pixelToLight);
		float   attenuation = 1 - (length(pixelToLight) * (1 / pointLights[j].radius));
		dColorAccumulator += (dColor + sColor) * saturate(attenuation);
		aColorAccumulator += pointLights[j].ambientColor;
	}

	for (int k = 0; k < SPOT_LIGHT_COUNT; k++)
	{
		float3  pixelToLight = spotLights[k].position - pixel.positionT;
		float4	dColor = diffuseColor(spotLights[k].diffuseColor, spotLights[k].ambientColor, pixel.normal, pixelToLight);
		float4  sColor = specularColor(eyePosition, pixel.positionT, pixel.normal, -pixelToLight);

		float3	directionToLight = normalize(pixelToLight);
		float   linearAttenuation = lerp(1.0f, 0.0f, length(pixelToLight) / spotLights[k].range);

		float   phi = radians(spotLights[k].radius);
		float	theta = radians(spotLights[k].radius * 0.75f);
		float	cosAlpha = max(0.0f, (dot(directionToLight, normalize(-spotLights[k].direction))));

		float	totalAttenuation = 0.0f;
		if (cosAlpha > theta) {
			totalAttenuation = 1.0f;
		}
		else if (cosAlpha > phi) { // Arbitrary Phi value
			totalAttenuation = pow((cosAlpha - phi) / (theta - phi), 32);
		}

		totalAttenuation *= linearAttenuation;
		dColorAccumulator += (dColor + sColor) * saturate(totalAttenuation);
		aColorAccumulator += spotLights[k].ambientColor;
	}

	float4 textureColor = diffuseTexture.Sample(diffuseSampler, pixel.uv);
	return (dColorAccumulator + aColorAccumulator) * textureColor;
}


