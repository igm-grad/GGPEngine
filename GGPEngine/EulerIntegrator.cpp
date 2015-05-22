#include "EulerIntegrator.h"
#include <DirectXMath.h>

EulerIntegrator::EulerIntegrator()
{
}


EulerIntegrator::EulerIntegrator(GameTimer time)
{
	timer = time;
}


void EulerIntegrator::setTimer(GameTimer time)
{
	timer = time;
}


EulerIntegrator::~EulerIntegrator()
{
}


void EulerIntegrator::IntegrateGameObject(RigidBody& gameObject)
{
	// Performing Linear Integration.

	XMVECTOR position = XMLoadFloat3(&(gameObject.getPosition()));								// Current Position of the Object (P)
	XMVECTOR velocity = XMLoadFloat3(&(gameObject.getVelocity()));								// Current Velocity of the Object (V)

	position = position + (velocity * timer.DeltaTime());										// Euler Integration: P(k+1) = P(k) + (h * V(k))

	XMFLOAT3 PositionToBeStored;
	XMStoreFloat3(&PositionToBeStored, position);

	gameObject.setPosition(PositionToBeStored);													// Store the new Position

	// Performing Rotational Integration.

	XMVECTOR q = XMLoadFloat4(&(gameObject.getOrientation()));									// Quaternion Representing the current rotation
	XMFLOAT3 angularVelocity = gameObject.getAngularVelocity();									// Angular velocity in Euler Angles
	XMVECTOR w = XMVectorSet(angularVelocity.x, angularVelocity.y, angularVelocity.z, 0.0f);	// Angular velocity in Quaternions ( omega 'w')
	
	XMVECTOR derivative = (1.0f / 2.0f)* w * q;													// Quaternion Derivative formula: q' = (1/2)wq
	
	q = q + (derivative * timer.DeltaTime());													// Euler Integration: q(k+1) = q(k) + (h * q'(k))
	
	XMFLOAT4 quaternionTobeStored;
	XMStoreFloat4(&quaternionTobeStored, q);

	gameObject.setOrientation(quaternionTobeStored);											// Store the new orientation.

}
