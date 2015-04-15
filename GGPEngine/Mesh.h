#pragma once
#include <d3d11.h>
#include "Vertex.h"
#include "MeshLoader.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

class  Mesh
{
protected:
	ID3D11Buffer*	vertexBuffer;
	ID3D11Buffer*	indexBuffer;
	int				indexCount;

	Mesh(Vertex* vertices, int vertexCount, UINT* indices, int indexCount, ID3D11Device* device);
	Mesh(const char* filename, ID3D11Device* device);
	Mesh();
	~Mesh();

	void SetVertexBuffer(Vertex* vertices, int vertexCount, ID3D11Device* device);
	void SetIndexBuffer(UINT* indices, int indexCount, ID3D11Device* device);

	ID3D11Buffer* const* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();

	friend class RenderEngine;
	friend class GameObject;
};

