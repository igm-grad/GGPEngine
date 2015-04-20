#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>
#include <Windows.h>


Material** mats;
int MATERIAL_COUNT = 3;
int MATERIAL_INDEX = 0;

void keyDownRightCallback(GameObject& gameObject)
{
	MATERIAL_INDEX = min(MATERIAL_INDEX + 1, MATERIAL_COUNT - 1);
	gameObject.material = mats[MATERIAL_INDEX];
}

void keyDownLeftCallback(GameObject& gameObject)
{
	MATERIAL_INDEX = max(MATERIAL_INDEX - 1, 0);
	gameObject.material = mats[MATERIAL_INDEX];
}

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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine * engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	Material* debugMaterial = engine->BasicMaterial();

	Material* diffuseMaterial = engine->DiffuseMaterial();
	diffuseMaterial->SetResource(L"Textures/DiffuseTexture1.JPG", "diffuseTexture");

	Material* diffuseNormalMaterial = engine->DiffuseNormalMaterial();
	diffuseNormalMaterial->SetResource(L"Textures/DiffuseTexture2.JPG", "diffuseTexture");
	diffuseNormalMaterial->SetResource(L"Textures/NormalTexture2.JPG", "normalTexture");

	mats = new Material*[MATERIAL_COUNT];
	mats[0] = debugMaterial;
	mats[1] = diffuseMaterial;
	mats[2] = diffuseNormalMaterial;

	GameObject* gameObject = engine->Sphere();
	gameObject->material = mats[MATERIAL_INDEX];

	gameObject->behavior = engine->CreateBehavior();
	gameObject->behavior->SetCallbackForKeyDown(keyDownRightCallback, KEYCODE_RIGHT);
	gameObject->behavior->SetCallbackForKeyDown(keyDownLeftCallback, KEYCODE_LEFT);
	gameObject->behavior->SetCallbackForKey(keyWCallback, KEYCODE_W);
	gameObject->behavior->SetCallbackForKey(keyACallback, KEYCODE_A);
	gameObject->behavior->SetCallbackForKey(keySCallback, KEYCODE_S);
	gameObject->behavior->SetCallbackForKey(keyDCallback, KEYCODE_D);
	gameObject->behavior->SetCallbackForKey(keyXCallback, KEYCODE_X);
	gameObject->behavior->SetCallbackForKey(keyYCallback, KEYCODE_Y);
	gameObject->behavior->SetCallbackForKey(keyZCallback, KEYCODE_Z);

	DirectionalLight* directionalLight = engine->CreateDirectionalLight(DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), DirectX::XMFLOAT3(1.f, -1.f, 1.f));
	PointLight *pointLight = engine->CreatePointLight(DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), DirectX::XMFLOAT3(0.f, 5.f, -5.f), 15.0f);
	SpotLight* spotLight = engine->CreateSpotLight(DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), DirectX::XMFLOAT3(0.0, 1.0f, 1.0), DirectX::XMFLOAT3(0.0f, -5.0f, -5.0f), 30.0f, 10.f);
	
	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}
}