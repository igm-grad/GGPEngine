#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>
#include <time.h> 

#pragma region Win32 Entry Point (WinMain)

int health = 80;
CoreEngine * engine;

Material** mats;
int MATERIAL_COUNT = 3;
int MATERIAL_INDEX = 0;

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	#pragma region Define materials
	Material* debugMaterial = engine->BasicMaterial();

	Material* diffuseMaterial = engine->DiffuseMaterial();
	diffuseMaterial->SetResource(L"Textures/DiffuseTexture1.JPG", "diffuseTexture");
	diffuseMaterial->specularExponent = 128.f;

	Material* particleMaterial = engine->ParticleMaterial();
	particleMaterial->SetResource(L"Textures/DiffuseTexture2.JPG", "diffuseTexture");

	mats = new Material*[MATERIAL_COUNT];
	mats[0] = debugMaterial;
	mats[1] = diffuseMaterial;
	mats[2] = particleMaterial;
	#pragma endregion

	GameObject* gameObject = engine->CreateGameObject("Models\\Lego.obj");
	//gameObject->material = engine->BasicMaterial();
	gameObject->material = mats[MATERIAL_INDEX];
	gameObject->transform->position.y = -20000;

	/*GameObject* rainGameObject = engine->CreateGameObject();

	//Create the ParticleSystem & add it to the target game object.
	ParticleSystem* Rain = engine->CreateParticleSystemThenAdd(rainGameObject, "Rain");
	*/

	engine->CreateParticleSystem(mats[2], 50);

	time_t startPoint = time(NULL);
		
	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}
}
