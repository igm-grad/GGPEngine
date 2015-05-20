#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>

Camera* camera = __nullptr;
GameObject* torus;
GameObject* boy;
GameObject* plane;

float boyY;
float torusY;

double totaltime = 0;

void MoveBack(GameObject& gameObject) {
	boy->transform->position.z -= 0.1f;
	torus->transform->position.z -= 0.1f;
}

void MoveForward(GameObject& gameObject) {
	boy->transform->position.z += 0.1f;
	torus->transform->position.z += 0.1f;
}

void MoveUp(GameObject& gameObject) {
	boyY += 0.1f;
	torusY += 0.1f;
}

void MoveDown(GameObject& gameObject) {
	boyY -= 0.1f;
	torusY -= 0.1;
}

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

void renderCallbackPlane(GameObject& plane, double secondsElapsed)
{
	plane.material->time += 1.f;
	
	totaltime = plane.material->time;
	float mult = 0.01f;
	torus->transform->position.y = sin(totaltime * mult)*1.75f - torusY;
	boy->transform->position.y = sin(totaltime * mult)*1.75f - boyY;

	//torus->transform->RotatePitch(sin(totaltime * mult)*XM_PI);
	//boy->transform->RotatePitch(sin(totaltime* mult)*XM_PI);
}


#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	boyY = -1.0f;
	torusY = -1.4f; 
	
	CoreEngine * engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	engine->CreateCubemap(L"Textures\\Skybox.dds");

	plane = engine->Plane(100.f, 100.f, 100.f, 100.f);
	plane->transform->position.y = -2.f;
	plane->transform->position.z = 18.15f;
	plane->transform->RotatePitch(-90.f);
	plane->transform->scale.x = 2.0f;

	boy = engine->CreateGameObject("Models\\boy.md5mesh");
	boy->material = engine->BasicMaterial();
	engine->LoadAnimation(boy, "Models\\boy.md5anim");

	boy->transform->scale.x = 0.03f;
	boy->transform->scale.y = 0.03f;
	boy->transform->scale.z = 0.03f;
	boy->transform->position.z = 0.5f;
	boy->transform->position.y = boyY;

	Material* diffuseMaterial2 = engine->DiffuseFluidMaterial();
	diffuseMaterial2->SetResource(L"Textures/DiffuseWater1.jpg", "diffuseTexture");
	diffuseMaterial2->SetResource(L"Textures/NormalWater1.jpg", "normalTexture");

	diffuseMaterial2->specularExponent = 128.f;

	plane->material = diffuseMaterial2;

	DirectionalLight* directionalLight = engine->CreateDirectionalLight(DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), DirectX::XMFLOAT3(0.0, 0.0f, -1.0));


	plane->behavior = engine->CreateBehavior();
	plane->behavior->renderCallback = renderCallbackPlane;
	plane->behavior->SetCallbackForKey(CameraKeyUpCallBack, KEYCODE_UP);
	plane->behavior->SetCallbackForKey(CameraKeyDownCallBack, KEYCODE_DOWN);
	plane->behavior->SetCallbackForKey(CameraKeyRightCallBack, KEYCODE_RIGHT);
	plane->behavior->SetCallbackForKey(CameraKeyLeftCallBack, KEYCODE_LEFT);
	plane->behavior->SetCallbackForKey(CameraKeyWCallBack, KEYCODE_W);
	plane->behavior->SetCallbackForKey(CameraKeySCallBack, KEYCODE_S);
	plane->behavior->SetCallbackForKey(CameraKeyACallBack, KEYCODE_A);
	plane->behavior->SetCallbackForKey(CameraKeyDCallBack, KEYCODE_D);

	plane->behavior->SetCallbackForKey(MoveUp, KEYCODE_NUMPAD8);
	plane->behavior->SetCallbackForKey(MoveBack, KEYCODE_NUMPAD6);
	plane->behavior->SetCallbackForKey(MoveDown, KEYCODE_NUMPAD4);
	plane->behavior->SetCallbackForKey(MoveForward, KEYCODE_NUMPAD2);


	float tempAmplitude = 1.75f;
	float tempFrequency = 0.01f;
	plane->material->SetVSFloat(tempAmplitude, "amplitude");
	plane->material->SetVSFloat(tempFrequency, "frequency");

	GameObject* sphere = engine->Sphere();
	sphere->transform->position = XMFLOAT3(3.0f, 3.0f, 0.0f);
	Material* sphereMaterial = engine->DiffuseMaterial();
	sphereMaterial->specularExponent = 0.0f;

	sphereMaterial->SetResource(L"Textures/SunMaterial.jpg", "diffuseTexture");
	sphere->material = /*engine->BasicMaterial();*/ sphereMaterial;

	torus = engine->Torus();
	Material* torusMaterial = engine->DiffuseMaterial();
	torusMaterial->SetResource(L"Textures/OrangeWhite.jpg", "diffuseTexture");
	torusMaterial->specularExponent = 0.0f;
	torus->material = /*engine->BasicMaterial();*/torusMaterial;
	torus->transform->position.z = 0.5f;
	torus->transform->position.y = torusY;
	torus->transform->RotatePitch(-400.0f);

	camera = engine->GetDefaultCamera();
	//camera->transform->rotation.x = XM_PI / 4.0f;

	/*
	//Set a .html file ad overlayer UI
	engine->InitializeUI("file:///./UI/main.html");
	//Set the JS function app.skill to invoke a the C++ function Attack
	engine->UIRegisterJavascriptFunction("moveup", MoveUp);
	engine->UIRegisterJavascriptFunction("movedown", MoveDown);
	engine->UIRegisterJavascriptFunction("moveback", MoveBack);
	engine->UIRegisterJavascriptFunction("movefoward", MoveFoward);
	*/

	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}

	delete(engine);
}