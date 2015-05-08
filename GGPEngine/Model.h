#pragma once
#include "Material.h"
#include "Mesh.h"
#include <vector>

struct Joint
{
	std::wstring name;
	int parentID;

	XMFLOAT3 pos;
	XMFLOAT4 orientation;
};

struct Weight
{
	int jointID;
	float bias;
	XMFLOAT3 pos;
};

class Model
{
private:
	std::vector<Mesh*> meshes;
	std::vector<Joint*> joints;

public:
	Model();
	~Model();
};

