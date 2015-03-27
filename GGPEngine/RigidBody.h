#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class RigidBody
{
public:
	RigidBody();
	~RigidBody();

	void recalculate();

private:

	// primary physics state

	XMFLOAT3	position;				// the position of the rigidbody center of mass in world coordinates (meters).
	XMFLOAT3	momentum;               // the momentum of the rigidbody in kilogram meters per second.
	XMFLOAT4	orientation;			// the orientation of the rigidbody represented by a unit quaternion.
	XMFLOAT3	angularMomentum;        // angular momentum vector of the rigidbody.

	// secondary state

	XMFLOAT3	velocity;               // velocity in meters per second (calculated from momentum).
	XMFLOAT4	spin;					// quaternion rate of change in orientation.
	XMFLOAT3	angularVelocity;        // angular velocity (calculated from angularMomentum).

	// constant state

	float		size;                   // length of the rigidbody sides in meters.
	float		mass;                   // mass of the rigidbody in kilograms.
	float		inverseMass;            // inverse of the mass used to convert momentum to velocity.
	float		inertia;				// inertia tensor of the rigidbody (i have simplified it to a single value due to the mass properties a rigidbody).
	float		inverseInertia;			// inverse inertia tensor used to convert angular momentum to angular velocity.
};

