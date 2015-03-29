#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine * engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	GameObject* gameObject = engine->Sphere();
	/*gameObject->material->SetResource(L"Textures/DiffuseTexture1.JPG", "diffuseTexture");*/
	Material* material = engine->DiffuseMaterial();
	material->SetResource(L"Textures/DiffuseTexture1.JPG", "diffuseTexture");
	//gameObject->material = material;
	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}
}