#pragma once
#include <d3d11.h>
#include "Vertex.h"

using namespace DirectX;

struct Face
{
	UINT pIndex[3];
	UINT nIndex[3];
	UINT uvIndex[3];
};

class Geometry
{
public:
	XMFLOAT3*	Positions;
	XMFLOAT3*	Normals;
	XMFLOAT3*	Tangents;
	XMFLOAT2*	UVs;
	UINT*		Indices;
	
	UINT		VertexCount;
	UINT		IndexCount;

	Geometry(XMFLOAT3* positions, XMFLOAT3* normals, XMFLOAT3* tangents, XMFLOAT2* uvs, UINT* indices, UINT vertexCount, UINT indexCount);
	Geometry();
	~Geometry();

	static Geometry* GeometryOBJ(const char* filename);
};

