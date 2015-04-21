#pragma once
#pragma once

#include <DirectXMath.h>

#include <string>

using namespace DirectX;

class Collider
{
public:
	Collider();
	~Collider();

	// initialise collider depending on type.
	virtual void init() = 0;

	// update collider, ie move it around with gameobject
	virtual void update() = 0;

	// check for collisions depending on type of Collider
	virtual bool checkCollsions() = 0;

	// get collider type
	virtual std::string colliderType() = 0;

protected:

	float		radius;
	XMFLOAT3	position;
	XMFLOAT3	scale;
};

