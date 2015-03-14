#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CoreEngine game(hInstance, prevInstance, cmdLine, showCmd);

	// If we can't initialize, we can't run
	if (!game.Initialize())
		return 0;

	// All set to run the game
	return 0; //game.Run();
}