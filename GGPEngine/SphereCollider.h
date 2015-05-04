#pragma once

#include "Collider.h"


class SphereCollider : public Collider
{
public:
	SphereCollider();
	~SphereCollider();

	// init sphere collider
	void init(GameObject* gameObject) override;

	// update position of collider
	void update() override;

	// check sphere/sphere collisions
	virtual bool checkCollisions(Collider* gameObjectCollider) override;

	std::string ColliderType() override { return colliderType; }

	float findMax(XMFLOAT3 gameObjectScale);

private:

	std::string colliderType;
};

