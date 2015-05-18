cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}; 

struct VStoGS
{
	float3	position	: POSITION;
	float4	color		: COLOR;
	float	size		: TEXCOORD1;
};


struct GStoPS
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	//float2 uv			: TEXCOORD0;
};


[maxvertexcount(3)]
void main(point VStoGS input[1], inout TriangleStream<GStoPS> outStream)
{
	//Note to self: Referring to input[] should be input[0] because you only have 1 point coming in. 
	//		Attempting input[i] will cause problems in a for loop.

	GStoPS output;

	// Offsets for smaller triangles
	float4 offsets[3];
	offsets[0] = float4(0, +0.1f, -0.1f, 0);
	offsets[1] = float4(+0.1f, -0.1f, -0.1f, 0);
	offsets[2] = float4(-0.1f, -0.1f, -0.1f, 0);

	// Calculate world view proj matrix
	matrix wvp = mul(mul(world, view), projection);

	[unroll]
	for (uint i = 0; i < 3; i++)
	{
		// Create a single vertex and add to the stream
		output.position = mul(float4(input[0].position, 1.0f), wvp);
		
		// Depth stuff
		float depthChange = output.position.z / output.position.w;

		//Adjust based on depth? This is for billboarding it seems
		output.position.xy += offsets[i].xy * depthChange * input[0].size;

		//Pass on the color
		output.color = input[0].color;
		
		// End append
		outStream.Append(output);
	}
}