struct Vertex
{
	float4 tangent	: TEXCOORD;
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD1;
};

struct Pixel
{
	float4 position	: SV_POSITION;
	float3 normal	: NORMAL;
	float3 positionT: TEXCOORD;
	float3 tangent	: TEXCOORD1;
	float3 bitangent: TEXCOORD2;
	float2 uv		: TEXCOORD3;
};

cbuffer transform : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}

Pixel main(Vertex vertex)
{
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