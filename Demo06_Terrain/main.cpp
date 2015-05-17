#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>

#pragma region Win32 Entry Point (WinMain)

// Keyboard Input Callback functions:
// Movement:
void keyWCallback(GameObject& gameObject)
{
	gameObject.transform->MoveForward();
}

void keySCallback(GameObject& gameObject)
{
	gameObject.transform->MoveBackward();
}

void keyACallback(GameObject& gameObject)
{
	gameObject.transform->MoveLeft();
}

void keyDCallback(GameObject& gameObject)
{
	gameObject.transform->MoveRight();
}

// Rotation
void keyICallback(GameObject& gameObject)
{
	gameObject.transform->RotatePitch(1.0f);
}

void keyOCallback(GameObject& gameObject)
{
	gameObject.transform->RotateYaw(1.0f);
}

void keyPCallback(GameObject& gameObject)
{
	gameObject.transform->RotateRoll(1.0f);
}




// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine * engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	// Creates Plane Object:
	GameObject* gameObject = engine->Terrain(50, 1024, 50, 1024);
	
	SpotLight* SptLght = engine->CreateSpotLight(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	// Color
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), // Difuse Coloer
		XMFLOAT3(0.0f, -1.0f, 0.0f), // Direction
		XMFLOAT3(0.0f, 10.0f, 0.0f), // Position
		50.0f, // Angle
		30.0f); // Range

	
	// Set Up the Textures for the Terrain
	gameObject->material = engine->loadHeightMap(L"../Demo06_Terrain/Textures/HeightMap.png");
	gameObject->material->SetResource(L"../Demo06_Terrain/Textures/RockTexture.png", "diffuseTexture");
	gameObject->material->SetResource(L"../Demo06_Terrain/Textures/RockNormal.png", "normalTexture");
	
	// Positions plane in center
	gameObject->transform->position = XMFLOAT3(0.0f, -1.0f, 0.0f);
	// Rotates plane so it is visible
	gameObject->transform->rotation.x = -3.1415f/ 16.0f;


	//Set Up Input Callbacks:
	// Creates Behavior
	gameObject->behavior = engine->CreateBehavior();
	// Atach fucntions to keys and to the GO Behavior
	gameObject->behavior->SetCallbackForKey(keyWCallback, KEYCODE_W);
	gameObject->behavior->SetCallbackForKey(keyACallback, KEYCODE_A);
	gameObject->behavior->SetCallbackForKey(keySCallback, KEYCODE_S);
	gameObject->behavior->SetCallbackForKey(keyDCallback, KEYCODE_D);
	gameObject->behavior->SetCallbackForKey(keyICallback, KEYCODE_I);
	gameObject->behavior->SetCallbackForKey(keyOCallback, KEYCODE_O);
	gameObject->behavior->SetCallbackForKey(keyPCallback, KEYCODE_P);

	// Increse movement Velocity to maintain my sanity.
	gameObject->transform->movementSpeed = 0.05f;

	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}
}