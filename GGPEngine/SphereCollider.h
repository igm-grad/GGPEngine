#pragma once

#include "Collider.h"

class SphereCollider : public Collider
{
public:
	SphereCollider();
	~SphereCollider();

	// init sphere collider
	void init();

	// update position of collider
	void update();

	// check sphere/sphere collisions
	bool checkCollisions();

	std::string ColliderType() { return colliderType; }

private:

	std::string colliderType;
};

