struct Vertex
{
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float3 tangent	: TEXCOORD;
	float2 uv		: TEXCOORD1;
};

struct Pixel
{
	float4 position	: SV_POSITION;
	float3 normal	: NORMAL;
	float3 positionT: TEXCOORD;
	float3 tangent	: TEXCOORD1
	float2 uv		: TEXCOORD2;
};

cbuffer transform : register(b0)
{
	matrix model;
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
	pixel.tangent = normalize(vertex.tangent);
	pixel.uv = vertex.uv;

	return pixel;
}