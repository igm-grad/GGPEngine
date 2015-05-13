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

	// Creates Plane Object:
	GameObject* gameObject = engine->Terrain(7.0f,10,10.0f,5);
	gameObject->material = engine->loadHeightMap();
	gameObject->material->SetResource(L"Textures/DiffuseTexture2.JPG", "diffuseTexture");
	gameObject->material->SetResource(L"Textures/NormalTexture2.JPG", "normalTexture");

	// Positions plane in center
	gameObject->transform->position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	// Rotates plane so it is visible
	gameObject->transform->rotation.x = -3.1415f/ 16.0f;

	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}	
}