#pragma once
#include "RigidBody.h"
#include <list>
#include <vector>

class Integrator
{
public:
	
	// The interface through which the programmer interacts with the integrator.
	// It allows the programmer to send the data in various formats.

	void IntegrateGameObjects(RigidBody* gameObjects, int size);
	void IntegrateGameObjects(std::list<RigidBody>& gameObjects);
	void IntegrateGameObjects(std::vector<RigidBody>& gameObjects);

protected:
	
	// The actual process of integration. In every Sub-Class, Only this function needs to be implemented.
	
	virtual void IntegrateGameObject(RigidBody &gameObject) = 0;
};

