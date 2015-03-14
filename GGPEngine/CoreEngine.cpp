#include "CoreEngine.h"


CoreEngine::CoreEngine()
{
	timer = GameTimer();
	gamePaused = false;
}


CoreEngine::~CoreEngine()
{
}

void CoreEngine::Initialize()
{

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
