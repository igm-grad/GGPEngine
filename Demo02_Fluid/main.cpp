#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>

void renderCallback(GameObject& gameObject, double secondsElapsed)
{
	gameObject.material->time += 1.f;
}

void keyXCallback(GameObject& gameObject)
{
	gameObject.transform->RotatePitch(1.0f);
}

void keyYCallback(GameObject& gameObject)
{
	gameObject.transform->RotateYaw(1.0f);
}

void keyZCallback(GameObject& gameObject)
{
	gameObject.transform->RotateRoll(1.0f);
}

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine * engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	GameObject* gameObject = engine->Plane(15.f, 50, 15.f, 50);
	gameObject->transform->position.y = -2.f;
	gameObject->transform->position.z = 2.5f;
	gameObject->transform->RotatePitch(-90.f);

	Material* diffuseMaterial2 = engine->DiffuseFluidMaterial();
	diffuseMaterial2->SetResource(L"Textures/water1.png", "diffuseTexture");
	diffuseMaterial2->specularExponent = 128.f;

	gameObject->material = diffuseMaterial2;
	
	DirectionalLight* directionalLight = engine->CreateDirectionalLight(DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), DirectX::XMFLOAT3(1.f, -1.f, 1.f));

	gameObject->behavior = engine->CreateBehavior();
	gameObject->behavior->renderCallback = renderCallback;
	gameObject->behavior->SetCallbackForKey(keyXCallback, KEYCODE_X);
	gameObject->behavior->SetCallbackForKey(keyYCallback, KEYCODE_Y);
	gameObject->behavior->SetCallbackForKey(keyZCallback, KEYCODE_Z);

	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}
}