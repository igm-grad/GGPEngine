#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>
#include <time.h> 

#pragma region Win32 Entry Point (WinMain)

int health = 80;
CoreEngine * engine;

//This function updated actively the UI by executing JS code
void UpdateBossHealthUI() {
	std::string javascriptStr = std::string("$('#progressbar').progressbar({ value: ") + std::to_string(health) + "}); ";
	engine->UIExecuteJavascript(javascriptStr);
}

//This function is a callback from the UI
//The JS function name registered to this callback is set with the engine->UIRegisterJavascriptFunction() method.
void Attack() {
	health -= 5;
	if (health < 0) health = 0;
	UpdateBossHealthUI();
}

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	engine = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);
	engine->Initialize();

	GameObject* gameObject = engine->CreateGameObject("Models\\Lego.obj");
	gameObject->material = engine->BasicMaterial();
	gameObject->transform->position.y = -2;

	time_t startPoint = time(NULL);

	//Set a .html file ad overlayer UI
	engine->InitializeUI("file:///./UI/test.html");
	//Set the JS function app.skill to invoke a the C++ function Attack
	engine->UIRegisterJavascriptFunction("skill", Attack);
		
	// Loop until we get a quit message from the engine
	while (!engine->exitRequested())
	{
		engine->Update();

		//Heal Health Bar every 2 sec
		time_t timer = time(NULL);
		int seconds = difftime(timer, startPoint);
		if (seconds >= 1) {
			health += 1;
			if (health > 100) health = 100;
			UpdateBossHealthUI();
			startPoint = timer;
		}

	}

	delete engine;
}
