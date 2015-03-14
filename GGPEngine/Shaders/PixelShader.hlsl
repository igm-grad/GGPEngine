
struct DirectionalLight
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 direction;
};

struct PointLight
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 position;
};

cbuffer lightBuffer : register( b0 )
{
	DirectionalLight light;
};

cbuffer pointLightBuffer : register(b1)
{
	PointLight pointLight;
};
// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 wPosition	: TEXCOORD1;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	//return float4(pointLight.position + 1, 1);
	
	float3 directionToLight = normalize(-light.direction);
	float directionalLightContribution = saturate(dot(input.normal, directionToLight));
	float4 light1 = (directionalLightContribution * light.diffuseColor) + light.ambientColor;

	float3 lightDirection = normalize(pointLight.position - (float3)input.wPosition);
	float pointLightContribution = saturate(dot(input.normal, lightDirection));
	float4 light2 = (pointLightContribution * pointLight.diffuseColor) + pointLight.ambientColor;

	return light2;
}