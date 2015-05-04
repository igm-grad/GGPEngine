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

	Pixel pixel;
	float wave = vertex.uv.y * (time * 0.0002f); // frequency
	vertex.position += (vertex.normal * sin(wave)) * 0.25f; // Amplitude
	pixel.position = mul(float4(vertex.position, 1.0f), clip);
	pixel.normal = mul(vertex.normal, (float3x3)world);
	pixel.positionT = mul(float4(vertex.position, 1.0f), world).xyz;
	pixel.uv = vertex.uv;

	return pixel;
}