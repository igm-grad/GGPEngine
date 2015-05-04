#pragma once

#include "GameObject.h"

#include <DirectXMath.h>

#include <string>

using namespace DirectX;

class Collider
{
public:
	Collider();
	~Collider();

	// initialise collider depending on type.
	virtual void init(GameObject* gameObject) = 0;

	// update collider, ie move it around with gameobject
	virtual void update() = 0;

	// check for collisions depending on type of Collider
	virtual bool checkCollisions(Collider* gameObjectCollider) = 0;

	// get collider type
	virtual std::string ColliderType() = 0;

	float		radius;
	XMFLOAT3	position;
	XMFLOAT3	scale;
	GameObject* gameObject;
};

