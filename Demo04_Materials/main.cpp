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

	/*gameObject->material = engine->DiffuseNormalMaterial();
	gameObject->material->SetResource(L"Textures/DiffuseTexture2.JPG", "diffuseTexture");
	gameObject->material->SetResource(L"Textures/NormalTexture2.JPG", "normalTexture");*/
	DirectionalLight* directionalLight = engine->CreateDirectionalLight(DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(1.f, -1.f, 1.f));
	PointLight *pointLight = engine->CreatePointLight(DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(5.f, 5.f, -5.f), 60.0f);
	SpotLight* spotLight = engine->CreateSpotLight(DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(0.0, 1.0f, 1.0), DirectX::XMFLOAT3(0.0f, -5.0f, -5.0f), 60.0f, 10.f);
	
	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();
	}
}