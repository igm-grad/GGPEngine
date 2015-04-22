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

	GameObject* gameObject = engine->Terrain(7.0f,10,10.0f,5);

	gameObject->material = engine->BasicMaterial();


	gameObject->transform->position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	gameObject->transform->rotation.x = -3.1415f/ 16.0f;

	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		//gameObject2->transform->MoveBackward();
		//gameObject1->transform->MoveBackward();
		engine->Update();
	}
	
}