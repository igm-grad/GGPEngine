#pragma once
#pragma warning( disable: 4251 )
#include "Model.h"

class MeshLoader
{
public:
	MeshLoader();
	~MeshLoader();
	
	//The Interface through which the dev can load a model
	bool loadModel(const char* filePath, Model* model);

	bool loadMD5AnimFile(const char* filePath, Model* model);

private:
	bool loadOBJfile	(const char* filePath, Model* model);			// to load OBJ model
	bool loadFBXfile	(const char* filePath, Model* model);			// to load FBX model
	bool loadMD5MeshFile(const char* filePath, Model* model);			// to load MD5mesh model

};

