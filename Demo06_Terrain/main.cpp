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
	GameObject* gameObject = engine->Terrain(10, 512, 10, 512);
	//gameObject->material = engine->BasicMaterial();

	
	SpotLight* SptLght = engine->CreateSpotLight(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	// Color
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), // Difuse Coloer
		XMFLOAT3(0.0f, -1.0f, 0.0f), // Direction
		XMFLOAT3(0.0f, 10.0f, 0.0f), // Position
		50.0f, // Angle
		30.0f); // Range


	gameObject->material = engine->loadHeightMap();
	gameObject->material->SetResource(L"Textures/DiffuseTexture2.jpg", "diffuseTexture");
	gameObject->material->SetResource(L"Textures/NormalTexture2.jpg", "normalTexture");
	
	gameObject->material->SetResource(L"Textures/HeightMap.png", "heightMap");

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