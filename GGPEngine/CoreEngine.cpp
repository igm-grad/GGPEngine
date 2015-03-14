#include "CoreEngine.h"
#include <time.h>

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
	if ( physics->Initialize()) {
		timer.Start();
		physics->currentTime = timer.TotalTime();
		return true;
	}
	return false;
}

void CoreEngine::Update()
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
		
		physics->Update(timer.TotalTime());
		//renderer->CalculateFrameStats(timer.TotalTime());
		//renderer->Update(timer.DeltaTime());
		// Render call DrawScene();
	}
}

GameObject* CoreEngine::createGameObject()
{

}

Mesh* CoreEngine::createMesh(const char* filename)
{

}