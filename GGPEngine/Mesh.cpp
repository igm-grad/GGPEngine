#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <vector>

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

//#MyChanges
// Creates a plane mesh in the X and Z axis given a width, height and vertex distribution along those axis.
Mesh::Mesh(int width, int vertexPerWidth, int depth, int vertexPerDepth, ID3D11Device* device)
{
	std::vector<Vertex> verts;           // Verts we're assembling
	std::vector<UINT> indices;           // Indices of these verts

	int IndicesIndex = 0;
	// Loop though the columns
	for (int k = 0; k < vertexPerDepth - 1; k++)
	{
		// Loop though the lines (x-axis)
		for (int j = 0; j < vertexPerWidth-1; j++) // May need to change to vertexperwidth.
		{// Creates a quad. Draw triangle in [k,j], [k+1,j] and [k,j+vertexPerWidth] and another in [k+1,j], [k+1,jj+vertexPerWidth+1] and [k,j+vertexPerWidth].
			
			int index = k*vertexPerWidth + j;
			//First Triangle
			// Vertices
			// #1
			verts[index].Position.x = j - width / 2;
			verts[index].Position.y = 0;
			verts[index].Position.z = k - depth / 2;
			indices.push_back(IndicesIndex++);
			// #2
			verts[index + 1].Position.x = j - width / 2;
			verts[index+1].Position.y = 0;
			verts[index + 1].Position.z = k - depth / 2;
			indices.push_back(IndicesIndex++);
			// #3
			verts[index + vertexPerWidth].Position.x = j - width / 2;
			verts[index + vertexPerWidth].Position.y = 0;
			verts[index + vertexPerWidth].Position.z = k - depth / 2;
			indices.push_back(IndicesIndex++);
		}
	}

	Mesh(&verts[0], verts.size(), &indices[0], indices.size(), device);
}

Mesh::Mesh() : indexCount(0)
{

}

Mesh::~Mesh()
{
	vertexBuffer->Release();
	indexBuffer->Release();
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