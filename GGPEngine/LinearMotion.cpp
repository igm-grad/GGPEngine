#include "LinearMotion.h"


LinearMotion::LinearMotion()
{
}


LinearMotion::~LinearMotion()
{
}

void LinearMotion::recalculate()
{
	XMVECTOR Momentum = XMLoadFloat3(&momentum);
	XMVECTOR Velocity = XMLoadFloat3(&velocity);

	Velocity = Momentum * inverseMass;

	XMStoreFloat3(&velocity, Velocity);
}