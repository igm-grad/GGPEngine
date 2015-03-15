#include "PhysicsEngine.h"
#include <cstdio>

PhysicsEngine::PhysicsEngine()
{
}


PhysicsEngine::~PhysicsEngine()
{
}

bool PhysicsEngine::Initialize()
{
	return true;
}

void PhysicsEngine::Update(float totalTime)
{
	double t = 0.0;
	double dt = 0.01;

	double accumulator = 0.0;

	double newTime = totalTime;

	double frameTime = newTime - currentTime;
	if (frameTime > 0.25)
	{
		frameTime = 0.25;
	}

	currentTime = newTime;
	accumulator += frameTime;

	while (accumulator >= dt)
	{
		// Previous state = currentState
		// Simulate Phyics
		printf("FixedUpdate \n");
		t += dt;
		accumulator -= dt;
	}

	const double alpha = accumulator / dt;
	// interpolate between previous phyics state and current physics state
}