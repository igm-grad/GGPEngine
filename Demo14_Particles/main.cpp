#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>
#include <time.h> 

#pragma region Win32 Entry Point (WinMain)

int health = 80;
CoreEngine * engine;

Material** mats;
int MATERIAL_COUNT = 1;
int MATERIAL_INDEX = 0;

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	//GameObject* ball = engine->Sphere();
	//ball->material = engine->BasicMaterial();
	//ball->transform->position = { 0, 0, 5 };
	//ball->transform->scale = {8,8,8};

	#pragma region Define materials
	Material* particleMaterial = engine->ParticleMaterial();
	//particleMaterial->SetResource(L"Textures/whitePuff00.PNG", "particleTexture");
	particleMaterial->SetResource(L"Textures/DiffuseTexture2.JPG", "particleTexture");
	particleMaterial->specularExponent = 128.f;

	mats = new Material*[MATERIAL_COUNT];
	mats[0] = particleMaterial;
	#pragma endregion

	GameObject* gameObject = engine->CreateGameObject("Models\\Lego.obj");
	//gameObject->material = engine->BasicMaterial();
	gameObject->material = mats[MATERIAL_INDEX];
	gameObject->transform->position.y = 0;

	engine->CreateParticleSystem(mats[0], 50);

	time_t startPoint = time(NULL);
		
	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}
}
