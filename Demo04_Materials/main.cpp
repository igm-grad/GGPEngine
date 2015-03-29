#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine * engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	GameObject* gameObject = engine->Sphere();
	gameObject->material = engine->DiffuseMaterial();
	gameObject->material->SetResource(L"Textures/DiffuseTexture1.JPG", "diffuseTexture");

	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}
}