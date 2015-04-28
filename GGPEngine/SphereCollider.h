#pragma once

#include "Collider.h"
#include "GameObject.h"

class SphereCollider : public Collider
{
public:
	SphereCollider();
	~SphereCollider();

	// init sphere collider
	void init(const GameObject& gameObject);

	// update position of collider
	void update(const GameObject& gameObject);

	// check sphere/sphere collisions
	bool checkCollisions(SphereCollider* gameObjectA, SphereCollider* gameObjectB);

	std::string ColliderType() { return colliderType; }

private:

	std::string colliderType;
};

