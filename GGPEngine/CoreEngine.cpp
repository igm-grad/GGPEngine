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
	Mesh* mesh = NULL; 
	GameObject* obj = new GameObject(mesh);

	return obj;
}

GameObject* CoreEngine::createGameObject(const char* filename)
{
	Mesh* mesh = createMesh(filename);
	GameObject* obj = new GameObject(mesh);

	return obj;
}

Mesh* CoreEngine::createMesh(const char* filename)
{
	std::unordered_map<std::string, Mesh*>::iterator it = meshIndex.find(filename);
	Mesh* meshObj;
	if (it == meshIndex.end())
	{
		// The mesh was not found in the meshIndex i.e It has not been loaded already
		meshObj = new Mesh(filename, renderer->getDevice());
	}
	else
	{
		// The object was found in the meshIndex i.e the mesh has already been loaded
		meshObj = it->second;
	}
	return meshObj;
}