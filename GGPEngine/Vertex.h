#pragma once
#include <DirectXMath.h>

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

// Wasted WAY too much time on this!
using namespace DirectX;

// Vertex struct for triangles
struct Vertex
{
	XMFLOAT4 Tangent;
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
};

// Vertex struct for triangles
struct ParticleVertex
{
	//bool		active;
	XMFLOAT3	position;
	float	spacerA;
	//float		age;
	XMFLOAT4	color;
	XMFLOAT3	size;
	float	spacerB;
	XMFLOAT3	velocity;
	float	spacerC;
};
