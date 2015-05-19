#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>

void keyWCallback(GameObject& gameObject)
{
	for (int i = 0; i < 33; i++)
		gameObject.transform->MoveForward();
}

void keySCallback(GameObject& gameObject)
{
	for (int i = 0; i < 33; i++)
		gameObject.transform->MoveBackward();
}

void keyACallback(GameObject& gameObject)
{
	for (int i = 0; i < 33; i++)
		gameObject.transform->MoveLeft();
}

void keyDCallback(GameObject& gameObject)
{
	for (int i = 0; i < 33; i++)
		gameObject.transform->MoveRight();
}

void keyXCallback(GameObject& gameObject)
{
	gameObject.transform->RotatePitch(33.0f);
}

void keyYCallback(GameObject& gameObject)
{
	gameObject.transform->RotateYaw(33.0f);
}

void keyZCallback(GameObject& gameObject)
{
	gameObject.transform->RotateRoll(33.0f);
}

#pragma region Win32 Entry Point (WinMain)
// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine * engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	GameObject* gameObject = engine->CreateGameObject("Models\\boy.md5mesh");
	gameObject->material = engine->BasicMaterial();
	engine->LoadAnimation(gameObject, "Models\\boy.md5anim");

	gameObject->transform->scale.x = 0.03f;
	gameObject->transform->scale.y = 0.03f;
	gameObject->transform->scale.z = 0.03f;

	gameObject->behavior = engine->CreateBehavior();
	gameObject->behavior->SetCallbackForKey(keyWCallback, KEYCODE_W);
	gameObject->behavior->SetCallbackForKey(keyACallback, KEYCODE_A);
	gameObject->behavior->SetCallbackForKey(keySCallback, KEYCODE_S);
	gameObject->behavior->SetCallbackForKey(keyDCallback, KEYCODE_D);
	gameObject->behavior->SetCallbackForKey(keyXCallback, KEYCODE_X);
	gameObject->behavior->SetCallbackForKey(keyYCallback, KEYCODE_Y);
	gameObject->behavior->SetCallbackForKey(keyZCallback, KEYCODE_Z);

	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}

	delete engine;
}