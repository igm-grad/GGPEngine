#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>

#pragma region Win32 Entry Point (WinMain)
int val = 100;

void UpdateBossHealth(CoreEngine* e) {
	auto javascript = std::string("$('#progressbar').progressbar({ value: ") + std::to_string(val) + "}); ";
	val--; if (val < 0) val = 100;
	e->UIExecuteJavascript(javascript);
}

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine * engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	GameObject* gameObject = engine->CreateGameObject("Models\\Lego.obj");
	gameObject->material = engine->BasicMaterial();

	engine->InitializeUI(L"file:///./UI/test.html");

	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();

		UpdateBossHealth(engine);
	}
}
