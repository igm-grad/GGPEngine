#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <vector>

#define ReleaseMacro(x) { if(x && ((unsigned int) x) != 0xfeeefeee){ x->Release(); x = 0; } }

Mesh::Mesh(Vertex* vertices, int vertexCount, UINT* indices, int indexCount, ID3D11Device* device)
{
	SetVertexBuffer(vertices, vertexCount, device);
	SetIndexBuffer(indices, indexCount, device);
}

Mesh::Mesh(const char* filename, ID3D11Device* device)
{
	std::vector<Vertex> verts;           // Verts we're assembling
	std::vector<UINT> indices;           // Indices of these verts

	MeshLoader meshLoader;
	std::string filePath = filename;
	if (!meshLoader.loadModel(filename, verts, indices))
	{
		return;							// If for some reason the mesh does not load, Bail out.
	}
	
	// - At this point, "verts" is a vector of Vertex structs, and can be used
	//    directly to create a vertex buffer:  &verts[0] is the first vert
	// - The vector "indices" is similar. It's a vector of unsigned ints and
	//    can be used directly for the index buffer: &indices[0] is the first int
	// - "triangleCounter" is BOTH the number of vertices and the number of indices
	SetVertexBuffer(&verts[0], verts.size(), device);
	SetIndexBuffer(&indices[0], indices.size(), device);
}

Mesh::Mesh() : indexCount(0)
{

}

Mesh::~Mesh()
{
	ReleaseMacro(vertexBuffer);
	ReleaseMacro(indexBuffer);
}

void Mesh::SetVertexBuffer(Vertex* vertices, int vertexCount, ID3D11Device* device)
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * vertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	device->CreateBuffer(&vbd, &vertexData, &vertexBuffer);
}

void Mesh::SetIndexBuffer(UINT* indices, int indexCount, ID3D11Device* device)
{
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	device->CreateBuffer(&ibd, &indexData, &indexBuffer);

	this->indexCount = indexCount;
}

ID3D11Buffer* const* Mesh::GetVertexBuffer()
{
	return &vertexBuffer;
}

ID3D11Buffer* Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

int Mesh::GetIndexCount()
{
	return indexCount;
}