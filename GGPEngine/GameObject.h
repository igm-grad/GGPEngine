#pragma once
#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include "Effects.h"
#include "ParticleSystem.h"
#include "Transform.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

using namespace DirectX;

class  GameObject
{
public:

	Transform*			transform;
	Mesh*				mesh;
	Material*			material;
	ParticleSystem*		particleSystem;
	Effects*			effects;

	GameObject(Mesh* mesh) : mesh(mesh) {};
	GameObject() : GameObject(NULL) {};
	~GameObject() { delete(mesh); delete(material); delete(transform);  };

};

