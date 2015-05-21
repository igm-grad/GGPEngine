#pragma once
#pragma warning( disable: 4251 )
#include <d3d11.h>
#include "Vertex.h"
#include "Model.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

class  Mesh
{
public:
	ID3D11Buffer*	vertexBuffer;
	ID3D11Buffer*	indexBuffer;
	int				indexCount;

	Mesh(Vertex* vertices, int vertexCount, UINT* indices, int indexCount, ID3D11Device* device);
	Mesh();
	~Mesh();

	void SetVertexBuffer(Vertex* vertices, int vertexCount, ID3D11Device* device);
	void SetIndexBuffer(UINT* indices, int indexCount, ID3D11Device* device);

	ID3D11Buffer* const* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();

	//For animation
	//int texArrayIndex;
	int numTriangles;

	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	std::vector<Weight> weights;

	std::vector<XMFLOAT3> positions;

	friend class MeshLoader;
	friend class RenderEngine;
	friend class GameObject;
};

