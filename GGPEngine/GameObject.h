#pragma once
#pragma warning( disable: 4251 )
#include <DirectXMath.h>
#include "Model.h"
#include "Material.h"
#include "Transform.h"
#include "Behavior.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

using namespace DirectX;

class  GameObject
{
public:

	UINT32		id;
	Transform*	transform;
	Model*		model;
	Material*	material;
	Behavior*	behavior;

	GameObject(Model* model) : model(model) {};
	GameObject() : GameObject(NULL) {};
	~GameObject() { delete(model); delete(material); delete(transform); /*delete(behavior);*/ };
};

