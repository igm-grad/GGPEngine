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
	float3	rotation : TEXCOORD;
	float3	position : POSITION;
	float3	scale  : TEXCOORD1;
	float3	forward : TEXCOORD2;
	float3	up : TEXCOORD3;
	float3	right : TEXCOORD4;
	float	movementSpeed : TEXCOORD5;

	//float4 position : POSITION;
	//float2 tex : TEXCOORD0;
	//float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
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

	// Store the texture coordinates for the pixel shader.
	output.tex = float2(0,0);

	// Store the particle color for the pixel shader. 
	output.color = float4(1,0,0,1);

	return output;
}