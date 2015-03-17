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


	MSG msg = { 0 };
	// Loop until we get a quit message from windows
	while (msg.message != WM_QUIT)
	{
		// Peek at the next message (and remove it from the queue)
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			// Handle this message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			e->Update();
		}
	}


}