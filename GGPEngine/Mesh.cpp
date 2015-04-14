#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

Mesh::Mesh(Vertex* vertices, int vertexCount, UINT* indices, int indexCount, ID3D11Device* device)
{
	SetVertexBuffer(vertices, vertexCount, device);
	SetIndexBuffer(indices, indexCount, device);
}

Mesh::Mesh(const char* filename, ID3D11Device* device)
{
	// File input object
	std::ifstream obj(filename); // <-- Replace filename with your parameter

	// Check for successful open
	if (!obj.is_open())
		return;

	// Variables used while reading the file
	std::vector<XMFLOAT3> positions;     // Positions from the file
	std::vector<XMFLOAT3> normals;       // Normals from the file
	std::vector<XMFLOAT2> uvs;           // UVs from the file
	std::vector<Vertex> verts;           // Verts we're assembling
	std::vector<UINT> indices;           // Indices of these verts
	std::map<int, std::vector<XMFLOAT3>> sharedTangentMap;
	std::map<int, std::vector<XMFLOAT3>> sharedBitangentMap;
	unsigned int triangleCounter = 0;    // Count of triangles/indices
	char chars[100];                     // String for line reading

	// Still good?
	while (obj.good())
	{
		// Get the line (100 characters should be more than enough)
		obj.getline(chars, 100);

		// Check the type of line
		if (chars[0] == 'v' && chars[1] == 'n')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 norm;
			sscanf_s(
				chars,
				"vn %f %f %f",
				&norm.x, &norm.y, &norm.z);

			// Add to the list of normals
			normals.push_back(norm);
		}
		else if (chars[0] == 'v' && chars[1] == 't')
		{
			// Read the 2 numbers directly into an XMFLOAT2
			XMFLOAT2 uv;
			sscanf_s(
				chars,
				"vt %f %f",
				&uv.x, &uv.y);

			// Add to the list of uv's
			uvs.push_back(uv);
		}
		else if (chars[0] == 'v')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 pos;
			sscanf_s(
				chars,
				"v %f %f %f",
				&pos.x, &pos.y, &pos.z);

			// Add to the positions
			positions.push_back(pos);
		}
		else if (chars[0] == 'f')
		{
			// Read the 9 face indices into an array
			unsigned int i[9];
			sscanf_s(
				chars,
				"f %d/%d/%d %d/%d/%d %d/%d/%d",
				&i[0], &i[1], &i[2],
				&i[3], &i[4], &i[5],
				&i[6], &i[7], &i[8]);

			// - Create the verts by looking up
			//    corresponding data from vectors
			// - OBJ File indices are 1-based, so
			//    they need to be adusted
			Vertex v1;
			v1.Position = positions[i[0] - 1];
			v1.UV = uvs[i[1] - 1];
			v1.Normal = normals[i[2] - 1];

			Vertex v2;
			v2.Position = positions[i[3] - 1];
			v2.UV = uvs[i[4] - 1];
			v2.Normal = normals[i[5] - 1];

			Vertex v3;
			v3.Position = positions[i[6] - 1];
			v3.UV = uvs[i[7] - 1];
			v3.Normal = normals[i[8] - 1];

			// Add the verts to the vector
			verts.push_back(v1);
			verts.push_back(v2);
			verts.push_back(v3);

			// Add three more indices
			indices.push_back(triangleCounter++);
			indices.push_back(triangleCounter++);
			indices.push_back(triangleCounter++);

			float x1 = v2.Position.x - v1.Position.x;
			float x2 = v3.Position.x - v1.Position.x;
			float y1 = v2.Position.y - v1.Position.y;
			float y2 = v3.Position.y - v1.Position.y;
			float z1 = v2.Position.z - v1.Position.z;
			float z2 = v3.Position.z - v1.Position.z;

			float s1 = v2.UV.x - v1.UV.x;
			float s2 = v3.UV.x - v1.UV.x;
			float t1 = v2.UV.y - v1.UV.y;
			float t2 = v3.UV.y - v1.UV.y;
		
			float r = 1.0f / ((s1 * t2) - (s2 * t1));
			XMFLOAT3 tangent =		{ (((t2 * x1) - (t1 * x2)) * r), (((t2 * y1) - (t1 * y2)) * r), (((t2 * z1) - (t1 * z2)) * r) };
			XMFLOAT3 bitangent =	{ (((s2 * x1) - (s1 * x2)) * r), (((s2 * y1) - (s1 * y2)) * r), (((s2 * z1) - (s1 * z2)) * r) };

			for (int j = 3; j >= 0; j--) {
				int index = triangleCounter - j;
				if (sharedTangentMap.find(index) == sharedTangentMap.end()) {
					std::vector<XMFLOAT3> tangents = { tangent };
					std::vector<XMFLOAT3> bitangents = { bitangent };
					sharedTangentMap.insert({ index, tangents });
					sharedBitangentMap.insert({ index, bitangents });
				}
				else {
					sharedTangentMap.at(index).push_back(tangent);
					sharedBitangentMap.at(index).push_back(bitangent);
				}
			}

		}
	}

	for (int i = 0; i < (int)verts.size(); i++)
	{
		std::vector<XMFLOAT3>& faceTangents = sharedTangentMap.at(i);
		std::vector<XMFLOAT3>& faceBitangents = sharedBitangentMap.at(i);
		XMVECTOR vertexTangent = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR vertexBitangent = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR vertexNormal = XMLoadFloat3(&verts[i].Normal);

		for (int j = 0; j < (int)faceTangents.size(); j++) {
			XMVECTOR faceTangent = XMLoadFloat3(&faceTangents[j]);
			vertexTangent += faceTangent;

			XMVECTOR faceBitangent = XMLoadFloat3(&faceBitangents[j]);
			vertexBitangent += faceBitangent;
		}
		vertexBitangent /= (float)faceBitangents.size();
		vertexTangent = XMVector3Normalize(vertexTangent / (float)faceTangents.size());
		vertexTangent = XMVector3Normalize((vertexTangent - vertexNormal * XMVector3Dot(vertexNormal, vertexTangent)));
		XMStoreFloat4(&verts[i].Tangent, vertexTangent);

		XMStoreFloat(&verts[i].Tangent.w, XMVector3Dot((XMVector3Cross(vertexNormal, vertexTangent)), vertexBitangent));
		verts[i].Tangent.w = (verts[i].Tangent.w < 0.0f) ? -1.0f : 1.0f;
	}

	// Close
	obj.close();

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