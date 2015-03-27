#include "Integrator.h"


void Integrator::IntegrateGameObjects(RigidBody* gameObjects, int size)
{
	for (int i = 0; i < size; i++)
	{
		IntegrateGameObject(gameObjects[i]);
	}
}

void Integrator::IntegrateGameObjects(std::list<RigidBody>& gameObjects)
{
	std::list<RigidBody>::iterator i;

	for (i = gameObjects.begin(); i != gameObjects.end(); i++)
	{
		IntegrateGameObject(*i);
	}
}

void Integrator::IntegrateGameObjects(std::vector<RigidBody>& gameObjects)
{
	std::vector<RigidBody>::iterator i;

	for (i = gameObjects.begin(); i != gameObjects.end(); i++)
	{
		IntegrateGameObject(*i);
	}
}