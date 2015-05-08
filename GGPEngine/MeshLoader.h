#pragma once
#include "Vertex.h"
#include <vector>
#include <d3d11.h>
#include <fstream>

#ifdef _WINDLL
#include <fbxsdk.h>
#else
class FbxNode;
#endif

class MeshLoader
{
public:
	MeshLoader();
	~MeshLoader();
	
	//The Interface through which the dev can load a model
	bool loadModel(const char* filePath, std::vector<Vertex>& verts, std::vector<UINT>& indices);

private:
	bool loadOBJfile	(const char* filePath, std::vector<Vertex>& verts, std::vector<UINT>& indices);			// to load OBJ model
	bool loadFBXfile	(const char* filePath, std::vector<Vertex>& verts, std::vector<UINT>& indices);			// to load FBX model
	bool loadMD5MeshFile(const char* filePath, std::vector<Vertex>& verts, std::vector<UINT>& indices);			// to load MD5mesh model

};

