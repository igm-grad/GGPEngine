#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>
#include <time.h> 

#pragma region Win32 Entry Point (WinMain)


void UpdateBossHealthUI(CoreEngine* e, int health) {
	std::string javascriptStr = std::string("$('#progressbar').progressbar({ value: ") + std::to_string(health) + "}); ";
	e->UIExecuteJavascript(javascriptStr);
}

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine * engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	GameObject* gameObject = engine->CreateGameObject("Models\\Lego.obj");
	gameObject->material = engine->BasicMaterial();
	gameObject->transform->position.y = -2;

	engine->InitializeUI(L"file:///./UI/test.html");

	time_t startPoint = time(NULL);
	int health = 50;

	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();

		//Heal Health Bar every 2 sec
		time_t timer = time(NULL);
		int seconds = difftime(timer, startPoint);
		if (seconds >= 2) {
			health += 1;
			if (health > 100) health = 100;
			UpdateBossHealthUI(engine, health);
			startPoint = timer;
		}

	}
}
