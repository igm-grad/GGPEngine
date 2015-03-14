#pragma once
#include <DirectXMath.h>

// Wasted WAY too much time on this!
using namespace DirectX;

// Vertex struct for triangles
struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
};