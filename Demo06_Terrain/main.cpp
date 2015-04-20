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

	GameObject* gameObject1 = engine->Terrain(4,4,4,4);
	gameObject1->material = engine->BasicMaterial();
	gameObject1->transform->position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	gameObject1->transform->rotation.y = -3.14 / 2;
	gameObject1->transform->scale.x = 10.0f;
	gameObject1->transform->scale.y = 10.0f;
	gameObject1->transform->scale.z = 10.0f;

	
	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		//gameObject2->transform->MoveBackward();
		//gameObject1->transform->MoveBackward();
		engine->Update();
	}
	
}