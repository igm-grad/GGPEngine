#pragma once
#pragma once
#include "Windows.h"
#include "RenderEngine.h"
#include "PhysicsEngine.h"
#include "GameTimer.h"

class CoreEngine
{
public:
	RenderEngine*	renderer;
	PhysicsEngine*	physics;

	GameTimer		timer;
	bool			gamePaused;


	CoreEngine();
	~CoreEngine();

	void Initialize();
	void Update(float deltaTime);
};

