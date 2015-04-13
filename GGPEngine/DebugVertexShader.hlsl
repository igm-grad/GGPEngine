struct Vertex
{
	float4 tangent	: TANGENT;
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD;
};

struct Pixel
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
};

cbuffer transform : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}

Pixel main( Vertex vertex )
{
	matrix clip = mul(mul(world, view), projection);

	Pixel pixel;
	pixel.position = mul(float4(vertex.position, 1.0f), clip);

	float3 normalColor = normalize(mul(vertex.normal, (float3x3)world));
	pixel.color = float4(normalColor, 1.0f);
	return pixel;

}