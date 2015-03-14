#include "CoreEngine.h"


CoreEngine::CoreEngine(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	timer = GameTimer();
	renderer = new RenderEngine(hInstance);
	physics = new PhysicsEngine();
	gamePaused = false;
}


CoreEngine::~CoreEngine()
{
}

void CoreEngine::Initialize()
{
	renderer->Initialize();
	physics->Initialize();
}

void CoreEngine::Update(float deltaTime)
{
	// No message, so continue the game loop
	timer.Tick();

	if (gamePaused)
	{
		Sleep(100);
	}
	else
	{
		// Standard game loop type stuff

		renderer->CalculateFrameStats(timer.TotalTime());
		// Physics call UpdateScene(timer.DeltaTime());
		// Render call DrawScene();
	}
}
