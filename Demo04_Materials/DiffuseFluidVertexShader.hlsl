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
	double time;
}

Pixel main(Vertex vertex)
{
	matrix clip = mul(mul(world, view), projection);

	float wave = (vertex.uv.y * 5) + (vertex.uv.x * 5) + (time * 0.0005f);			// frequency
	vertex.position += (vertex.normal * sin(wave)) * 0.5f;	// Amplitude

	Pixel pixel;
	pixel.position = mul(float4(vertex.position, 1.0f), clip);
	pixel.normal = mul(vertex.normal, (float3x3)world);
	pixel.positionT = mul(float4(vertex.position, 1.0f), world).xyz;
	pixel.tangent = vertex.tangent.xyz;
	pixel.bitangent = cross(vertex.normal, vertex.tangent.xyz) * vertex.tangent.w;
	pixel.uv = vertex.uv;

	return pixel;
}