#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine * engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	engine->CreateCubemap(L"Textures\\Skybox.dds");

	GameObject* gameObject = engine->Sphere();
	gameObject->material = engine->BasicMaterial();

	//GameObject* rainGameObject = engine->CreateGameObject();

	//Create the ParticleSystem & add it to the target game object.
	//ParticleSystem* Rain = engine->CreateParticleSystemThenAdd(rainGameObject, "Rain");
	
	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}

	delete engine;
}