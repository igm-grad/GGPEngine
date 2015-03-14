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

bool CoreEngine::Initialize()
{
	if (renderer->Initialize() && physics->Initialize()) {
		return true;
	}
	return false;
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
