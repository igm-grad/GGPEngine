#include "AngularMotion.h"


AngularMotion::AngularMotion()
{
}


AngularMotion::~AngularMotion()
{
}

void AngularMotion::recalculate()
{
	XMVECTOR AngularMomentum = XMLoadFloat3(&angularMomentum);
	XMVECTOR AngularVelocity = XMLoadFloat3(&angularVelocity);

	AngularVelocity = AngularMomentum * inverseInertia;
	XMStoreFloat3(&angularVelocity, AngularVelocity);

	XMVECTOR Orientation = XMLoadFloat4(&orientation);
	XMVECTOR Spin = XMLoadFloat4(&spin);

	Orientation = XMQuaternionNormalize(Orientation);

	XMVECTOR q = XMVectorSet(angularVelocity.x, angularVelocity.y, angularVelocity.z, 0);

	Spin = 0.5f * q * Orientation;
	XMStoreFloat4(&spin, Spin);
}