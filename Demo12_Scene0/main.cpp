#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>
#include <Windows.h>

#pragma region Win32 Entry Point (WinMain)

Camera* camera = __nullptr;

void CameraKeyUpCallBack(GameObject& gameObject)
{
	camera->transform->MoveForward();
}

void CameraKeyDownCallBack(GameObject& gameObject)
{
	camera->transform->MoveBackward();
}

void CameraKeyRightCallBack(GameObject& gameObject)
{
	camera->transform->MoveRight();
}

void CameraKeyLeftCallBack(GameObject& gameObject)
{
	camera->transform->MoveLeft();
}

void CameraKeyWCallBack(GameObject& gameObject)
{
	camera->transform->RotatePitch(-1.0f);
}

void CameraKeySCallBack(GameObject& gameObject)
{
	camera->transform->RotatePitch(1.0f);
}

void CameraKeyACallBack(GameObject& gameObject)
{
	camera->transform->RotateYaw(-1.0f);
}

void CameraKeyDCallBack(GameObject& gameObject)
{
	camera->transform->RotateYaw(1.0f);
}

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine * engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	camera = engine->GetDefaultCamera();
	camera->transform->rotation.x = XM_PI / 4.0f;

	GameObject *cameraController = engine->Cone();
	cameraController->material = engine->BasicMaterial();

	cameraController->behavior = engine->CreateBehavior();
	cameraController->behavior->SetCallbackForKey(CameraKeyUpCallBack, KEYCODE_UP);
	cameraController->behavior->SetCallbackForKey(CameraKeyDownCallBack, KEYCODE_DOWN);
	cameraController->behavior->SetCallbackForKey(CameraKeyRightCallBack, KEYCODE_RIGHT);
	cameraController->behavior->SetCallbackForKey(CameraKeyLeftCallBack, KEYCODE_LEFT);
	cameraController->behavior->SetCallbackForKey(CameraKeyWCallBack, KEYCODE_W);
	cameraController->behavior->SetCallbackForKey(CameraKeySCallBack, KEYCODE_S);
	cameraController->behavior->SetCallbackForKey(CameraKeyACallBack, KEYCODE_A);
	cameraController->behavior->SetCallbackForKey(CameraKeyDCallBack, KEYCODE_D);

	DirectionalLight* directionalLight = engine->CreateDirectionalLight(DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), DirectX::XMFLOAT3(1.f, -1.f, -1.f));

	GameObject* plane = engine->Plane(50.0f, 50, 15.0f, 15);
	plane->material = engine->DiffuseNormalMaterial();
	plane->material->SetResource(L"Textures/DiffuseSand.png", "diffuseTexture");
	plane->material->SetResource(L"Textures/NormalSand.png", "normalTexture");
	plane->material->specularExponent = 256.0f;
	plane->transform->position.y = -20.0f;

	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}
}