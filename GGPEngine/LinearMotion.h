#pragma once
#include "RigidBody.h"

#include <DirectXMath.h>

using namespace DirectX;

class LinearMotion : public RigidBody
{
public:
	LinearMotion();
	~LinearMotion();

	void recalculate();

private:
	XMFLOAT3 position;
	XMFLOAT3 momentum;

	XMFLOAT3 velocity;

	float mass;
	float inverseMass;
};

