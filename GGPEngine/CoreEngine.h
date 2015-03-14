#pragma once

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

#include "Windows.h"
#include "RenderEngine.h"
#include "PhysicsEngine.h"
#include "GameTimer.h"

class GPPEngineAPI CoreEngine
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

