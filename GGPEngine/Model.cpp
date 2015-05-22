#include "Model.h"
#include "Mesh.h"
#include "MeshLoader.h"

Model::Model(const char* filename, ID3D11Device* device)
{
	MeshLoader meshLoader;
	std::string filePath = filename;
	if (!meshLoader.loadModel(filename, this))
	{
		return;							// If for some reason the mesh does not load, Bail out.
	}

	for (UINT i = 0; i < meshes.size(); i++) {
		meshes[i]->SetVertexBuffer(&(meshes[i]->vertices[0]), meshes[i]->vertices.size(), device);
		meshes[i]->SetIndexBuffer(&(meshes[i]->indices[0]), meshes[i]->indices.size(), device);
	}

}

Model::~Model()
{
	for (UINT i = 0; i < meshes.size(); i++) {
		delete(meshes[i]);
	}
}
