#include "RigidBody.h"


RigidBody::RigidBody()
{
	size = 1;
	mass = 1;
	inverseMass = 1.0f / mass;
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	momentum = XMFLOAT3(0.0f, 0.0f, 0.0f);
	orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	angularMomentum = XMFLOAT3(0.0f, 0.0f, 0.0f);
	inertia = mass * size * size * 1.0f / 6.0f;
	inverseInertia = 1.0f / inertia;
	recalculate();
}


RigidBody::~RigidBody()
{
}

void RigidBody::recalculate()
{
	// load all vectors
	XMVECTOR Momentum = XMLoadFloat3(&momentum);
	XMVECTOR Velocity = XMLoadFloat3(&velocity);

	XMVECTOR AngularMomentum = XMLoadFloat3(&angularMomentum);
	XMVECTOR AngularVelocity = XMLoadFloat3(&angularVelocity);

	XMVECTOR Orientation = XMLoadFloat4(&orientation);
	XMVECTOR Spin = XMLoadFloat4(&spin);

	// calculate velocity
	Velocity = Momentum * inverseMass;

	// calculate angular velocity
	AngularVelocity = AngularMomentum * inverseInertia;

	// normalize orientation
	Orientation = XMQuaternionNormalize(Orientation);

	// calculate spin
	Spin = 0.5f * XMVectorSet(angularVelocity.x, angularVelocity.y, angularVelocity.z, 0) * Orientation;

	// store all calculated vectors 
	XMStoreFloat3(&velocity, Velocity);

	XMStoreFloat3(&angularVelocity, AngularVelocity);

	XMStoreFloat4(&spin, Spin);
}
