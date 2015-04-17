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

	GameObject* gameObject1 = engine->Sphere();
	gameObject1->material = engine->BasicMaterial();
	gameObject1->transform->position = XMFLOAT3(0.0f, 0.0f, 10.0f);
	gameObject1->transform->movementSpeed = 0.001f;

	GameObject* gameObject2 = engine->Sphere();
	gameObject2->material = engine->BasicMaterial();
	gameObject2->transform->position = XMFLOAT3(0.3f, 0.0f, 0.0f);
	gameObject1->transform->movementSpeed = 0.001f;

	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		//gameObject2->transform->MoveBackward();
		gameObject1->transform->MoveBackward();
		engine->Update();
	}
}