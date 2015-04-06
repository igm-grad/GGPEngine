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
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
};
