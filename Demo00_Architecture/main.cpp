#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine * e = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);

	e->Initialize();


	GameObject* o = e->Sphere();


	// Loop until we get a quit message from the engine
	while (!e->exitRequested())
	{
		e->Update();
	}


}