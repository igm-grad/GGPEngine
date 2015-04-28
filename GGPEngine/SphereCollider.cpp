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

float SphereCollider::findMax(XMFLOAT3 gameObjectScale)
{
	float temp_max = ((gameObjectScale.x + gameObjectScale.y + abs(gameObjectScale.x - gameObjectScale.y)) / 2);
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
	float displacementMag = displacement.x * displacement.x + displacement.y * displacement.y + displacement.z * displacement.z;

	// find largest of the scales in x, y, z
	float maxScaleA = findMax(gameObjectA->scale);
	float maxScaleb = findMax(gameObjectB->scale);

	// add the radius' of objects to compae with distance magnitude
	float radiusAB = gameObjectA->radius * gameObjectA->scale + gameObjectB->radius * gameObjectA->scale;
}