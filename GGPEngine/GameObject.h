#pragma once
#pragma warning( disable: 4251 )
#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include "Effects.h"
#include "Transform.h"
#include "Behavior.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

using namespace DirectX;

class ParticleSystem; //Forward declaration

class  GameObject
{
public:

	UINT32		id;
	Transform*	transform;
	Mesh*		mesh;
	Material*	material;
	Behavior*	behavior;
	//ParticleSystem* particleSystem;

	GameObject(Mesh* mesh) : mesh(mesh) {};
	GameObject() : GameObject(NULL) {};
	~GameObject() { delete(mesh); delete(material); delete(transform); /*delete(behavior);*/ };
};

