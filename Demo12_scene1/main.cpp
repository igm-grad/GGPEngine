#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>

float amplitude = 0.0f;
float frequency = 0.0f;

void renderCallback(GameObject& plane, double secondsElapsed)
{
	plane.material->time += 1.f;
}

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine * engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	GameObject* plane = engine->Plane(100.f, 100.f, 100.f, 100.f);
	plane->transform->position.y = -2.f;
	plane->transform->position.z = 50.f;
	plane->transform->RotatePitch(-90.f);
	plane->transform->scale.x = 2.0f;

	Material* diffuseMaterial2 = engine->DiffuseFluidMaterial();
	diffuseMaterial2->SetResource(L"Textures/DiffuseWater1.jpg", "diffuseTexture");
	diffuseMaterial2->SetResource(L"Textures/NormalWater1.jpg", "normalTexture");

	diffuseMaterial2->specularExponent = 128.f;

	plane->material = diffuseMaterial2;

	DirectionalLight* directionalLight = engine->CreateDirectionalLight(DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), DirectX::XMFLOAT3(0.0, 0.0f, -1.0));


	plane->behavior = engine->CreateBehavior();
	plane->behavior->renderCallback = renderCallback;

	float tempAmplitude = 2.0f;
	float tempFrequency = 0.001f;
	plane->material->SetVSFloat(tempAmplitude, "amplitude");
	plane->material->SetVSFloat(tempFrequency, "frequency");

	GameObject* sphere = engine->Sphere();
	sphere->transform->position = XMFLOAT3(3.0f, 3.0f, 0.0f);
	Material* sphereMaterial = engine->DiffuseMaterial();
	sphereMaterial->SetResource(L"Textures/SunMaterial.jpg", "diffuseTexture");
	sphere->material = /*engine->BasicMaterial();*/ sphereMaterial;

	GameObject* torus = engine->Torus();
	Material* torusMaterial = engine->DiffuseMaterial();
	torusMaterial->SetResource(L"Textures/OrangeWhite.png", "diffuseTexture");
	torus->material = /*engine->BasicMaterial();*/torusMaterial;
	torus->transform->position.z = -1.5f;
	torus->transform->position.y = -1.0f;
	torus->transform->RotatePitch(-300.0f);

	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}
}