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
	float3 tangent	: TANGENT;
	float3 bitangent: BINORMAL;
	float2 uv		: TEXCOORD;
};

SamplerState	omniSampler		: register(s0);
Texture2D		diffuseTexture	: register(t0);
Texture2D		normalTexture	: register(t1);
cbuffer			lights			: register (b0)
{
	DirectionalLight	directionalLights[DIRECTIONAL_LIGHT_COUNT];
	PointLight			pointLights[POINT_LIGHT_COUNT];
	SpotLight			spotLights[SPOT_LIGHT_COUNT];
	float3				eyePosition;
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
	return float4(1.f, 1.f, 1.f, 1.0f) * pow(specularContribution, 32);
}

float4 main(Pixel pixel) : SV_TARGET
{
	pixel.tangent = normalize(pixel.tangent);
	pixel.bitangent = normalize(pixel.bitangent);
	pixel.normal = normalize(pixel.normal);
	float3x3 TBN = 
	{	
		pixel.tangent.x, pixel.tangent.y, pixel.tangent.z,
		pixel.bitangent.x, pixel.bitangent.y, pixel.bitangent.z,
		pixel.normal.x, pixel.normal.y, pixel.normal.z 
	};

	float3 normal = normalTexture.Sample(omniSampler, pixel.uv).xyz;
	normal = normal * 255.f/128.f - 1.0f;
	normal = mul(normal, TBN);

	float4 colorAccumulator = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
		float4 dColor = diffuseColor(directionalLights[i].diffuseColor, directionalLights[i].ambientColor, normal, -directionalLights[i].direction);
			float4 sColor = specularColor(eyePosition, pixel.positionT, normal, directionalLights[i].direction);
			colorAccumulator += (dColor + directionalLights[i].ambientColor + sColor);
	}

	for (int j = 0; j < POINT_LIGHT_COUNT; j++) {
		float3  pixelToLight = pointLights[j].position - pixel.positionT;
			float4	dColor = diffuseColor(pointLights[j].diffuseColor, pointLights[j].ambientColor, normal, pixelToLight);
			float4  sColor = specularColor(eyePosition, pixel.positionT, normal, -pixelToLight);
			float   attenuation = 1 - (length(pixelToLight) * (1 / pointLights[j].radius));
		colorAccumulator += (dColor + pointLights[j].ambientColor + sColor) * saturate(attenuation);
	}

	for (int k = 0; k < SPOT_LIGHT_COUNT; k++)
	{
		float3  pixelToLight = spotLights[k].position - pixel.positionT;
		float4	dColor = diffuseColor(spotLights[k].diffuseColor, spotLights[k].ambientColor, normal, pixelToLight);
		float4  sColor = specularColor(eyePosition, pixel.positionT, normal, -pixelToLight);

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
		colorAccumulator += (dColor + spotLights[k].ambientColor + sColor) * saturate(totalAttenuation);
	}


	float4 textureColor = diffuseTexture.Sample(omniSampler, pixel.uv);
	return colorAccumulator * textureColor;
}