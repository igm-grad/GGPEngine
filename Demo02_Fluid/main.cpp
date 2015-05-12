#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>

float amplitude = 0.0f;
float frequency = 0.0f;

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

void keyUpArrowCallback(GameObject& gameObject)
{
	frequency += 0.001f;
	frequency = min(1.0f, frequency);
	gameObject.material->SetVSFloat(frequency, "frequency");
}

void keyDownArrowCallback(GameObject& gameObject)
{
	frequency -= 0.001f;
	frequency = max(0.0f, frequency);
	gameObject.material->SetVSFloat(frequency, "frequency");
}

void keyLeftArrowCallback(GameObject& gameObject)
{
	amplitude -= 0.005f;
	amplitude = max(0.0f, amplitude);
	gameObject.material->SetVSFloat(amplitude, "amplitude");
}

void keyRightArrowCallback(GameObject& gameObject)
{
	amplitude += 0.005f;
	amplitude = min(1.0f, amplitude);
	gameObject.material->SetVSFloat(amplitude, "amplitude");
}

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine * engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	GameObject* gameObject = engine->Plane(50.f, 100, 50.f, 100);
	gameObject->transform->position.y = -2.f;
	gameObject->transform->position.z = 50.f;
	gameObject->transform->RotatePitch(-90.f);

	Material* diffuseMaterial2 = engine->DiffuseFluidMaterial();
	diffuseMaterial2->SetResource(L"Textures/DiffuseWater1.jpg", "diffuseTexture");
	diffuseMaterial2->SetResource(L"Textures/NormalWater1.jpg", "normalTexture");

	diffuseMaterial2->specularExponent = 128.f;

	gameObject->material = diffuseMaterial2;
	
	SpotLight* spotLight = engine->CreateSpotLight(DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), DirectX::XMFLOAT3(0.0, -1.0f, 0.0), DirectX::XMFLOAT3(0.0f, 5.0f, 50.0f), 15.0f, 20.f);

	gameObject->behavior = engine->CreateBehavior();
	gameObject->behavior->renderCallback = renderCallback;
	gameObject->behavior->SetCallbackForKey(keyXCallback, KEYCODE_X);
	gameObject->behavior->SetCallbackForKey(keyYCallback, KEYCODE_Y);
	gameObject->behavior->SetCallbackForKey(keyZCallback, KEYCODE_Z);
	gameObject->behavior->SetCallbackForKeyDown(keyUpArrowCallback, KEYCODE_UP);
	gameObject->behavior->SetCallbackForKeyDown(keyDownArrowCallback, KEYCODE_DOWN);
	gameObject->behavior->SetCallbackForKey(keyLeftArrowCallback, KEYCODE_LEFT);
	gameObject->behavior->SetCallbackForKey(keyRightArrowCallback, KEYCODE_RIGHT);

	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}
}