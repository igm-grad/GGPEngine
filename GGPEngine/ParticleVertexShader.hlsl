// Credit:
// http://www.rastertek.com/dx11tut39.html

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////

struct VertexInputType
{
	float3	position	:SV_POSITION;
	float4	color		:COLOR;
	float3	size		:TEXCOORD0;
	float3	velocity	:TEXCOORD1;
	float	age			:TEXCOORD2;
	float2	tex			:TEXCOORD3;
};

struct PixelInputType
{
	float4	position	:SV_POSITION;
	float4	color		:COLOR;
	float3	size		:TEXCOORD0;
	float3	velocity	:TEXCOORD1;
	float	age			:TEXCOORD2;
	float2	tex			:TEXCOORD3;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	matrix clip = mul(mul(worldMatrix, viewMatrix), projectionMatrix);
	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(float4(input.position, 1), clip);

	// Store the particle color for the pixel shader. 
	output.color = input.color;

	// The size, vel & age are currently irrelevant
	output.size = input.size;
	output.velocity = input.velocity;
	output.age = input.age;

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	return output;
}