#pragma once
#include <d3d11.h>
#include "Vertex.h"

class Mesh
{
public:
	ID3D11Buffer*	vertexBuffer;
	ID3D11Buffer*	indexBuffer;
	int				indexCount;

	Mesh(Vertex* vertices, int vertexCount, UINT* indices, int indexCount, ID3D11Device* device);
	Mesh(char* filename, ID3D11Device* device);
	Mesh();
	~Mesh();

	void SetVertexBuffer(Vertex* vertices, int vertexCount, ID3D11Device* device);
	void SetIndexBuffer(UINT* indices, int indexCount, ID3D11Device* device);

	ID3D11Buffer* const* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();
};

