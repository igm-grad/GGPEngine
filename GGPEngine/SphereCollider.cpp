#include "SphereCollider.h"

SphereCollider::SphereCollider()
{
	colliderType = "SphereCollider";
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::init(const GameObject& gameObject)
{
	position = gameObject.transform->position;
	scale = gameObject.transform->scale;
	radius = 1.0f; // for now
}

void SphereCollider::update(const GameObject& gameObject)
{
	position = gameObject.transform->position;
}

bool SphereCollider::checkCollisions(SphereCollider* gameObjectA, SphereCollider* gameObjectB)
{
	// store in XMVECTOR
	XMVECTOR GameObjectAPosition = XMLoadFloat3(&gameObjectA->position);
	XMVECTOR GameObjectBPosition = XMLoadFloat3(&gameObjectB->position);

	// find distance between GameObject1 and GameObject2
	XMVECTOR Displacement = GameObjectBPosition - GameObjectAPosition;

	// find magnitude of displacemnt
	XMFLOAT3 displacement;
	XMStoreFloat3(&displacement, Displacement);
	float displacementMag = displacement.x * displacement.x + displacement.x * displacement.x + displacement.x * displacement.x
}