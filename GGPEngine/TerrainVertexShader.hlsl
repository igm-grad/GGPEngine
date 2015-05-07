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
	matrix clip = mul(mul(world, view), projection);

	
	float HeightPosition = heightMap.Sample(omniSampler, vertex.uv).r;
	vertex.position = float3 (vertex.position.x, HeightPosition.x, vertex.position.z).xyz;

	Pixel pixel;
	pixel.position = mul(float4(vertex.position, 1.0f), clip);
	pixel.normal = mul(vertex.normal, (float3x3)world);
	pixel.positionT = mul(float4(vertex.position, 1.0f), world).xyz;
	pixel.tangent = vertex.tangent.xyz;
	pixel.bitangent = cross(vertex.normal, vertex.tangent.xyz) * vertex.tangent.w;
	pixel.uv = vertex.uv;

	return pixel;
}