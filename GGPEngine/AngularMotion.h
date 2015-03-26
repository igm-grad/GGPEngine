#pragma once
#include "RigidBody.h"

#include <DirectXMath.h>

using namespace DirectX;

class AngularMotion : public RigidBody
{
public:
	AngularMotion();
	~AngularMotion();

	void recalculate();

private:
	XMFLOAT4 orientation;
	XMFLOAT3 angularMomentum;

	XMFLOAT4 spin;
	XMFLOAT3 angularVelocity;

	float inertia;
	float inverseInertia;
};

