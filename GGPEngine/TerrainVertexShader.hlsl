struct Vertex
{
	float4 tangent	: TANGENT;
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD;
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

cbuffer transform : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}

SamplerState	omniSampler		: register(s0);
Texture2D		heightMap		: register(t2);

Pixel main(Vertex vertex)
{	
	float HeightMapPosition = heightMap.SampleLevel(omniSampler, vertex.uv, 0).r;
	vertex.position.y += mul(HeightMapPosition, 2.0f);

	matrix clip = mul(mul(world, view), projection);
	
	Pixel pixel;
	pixel.position = mul(float4(vertex.position, 1.0f), clip);
	pixel.normal = mul(vertex.normal, (float3x3)world);
	pixel.positionT = mul(float4(vertex.position, 1.0f), world).xyz;
	pixel.tangent = vertex.tangent.xyz;
	pixel.bitangent = cross(vertex.normal, vertex.tangent.xyz) * vertex.tangent.w;
	pixel.uv = vertex.uv;

	return pixel;
}