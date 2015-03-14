#include "CoreEngine.h"
#include <time.h>

#pragma region Global Window Callback
namespace
{
	// Allows us to forward Windows messages from a global
	// window procedure to our member function window procedure
	// because we cannot assign a member function to WNDCLASS::lpfnWndProc
	CoreEngine* gCore = 0;
}

// Set up a global callback for handling windows messages
LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward the global callback to our game's message handling

	// Forward hwnd because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return gCore->MsgProc(hwnd, msg, wParam, lParam);
}
#pragma endregion


CoreEngine::CoreEngine(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	timer = GameTimer();
	renderer = new RenderEngine(hInstance, (WNDPROC) MainWndProc);
	physics = new PhysicsEngine();
	gamePaused = false;
	gCore = this;
}


CoreEngine::~CoreEngine()
{
}

bool CoreEngine::Initialize()
{
	if ( physics->Initialize()) {
		timer.Start();
		physics->currentTime = timer.TotalTime();
		return true;
	}
	return false;
}

void CoreEngine::Update()
{
	// No message, so continue the game loop
	timer.Tick();

	if (gamePaused) 
	{
		Sleep(100);
	}
	else 
	{
		// Standard game loop type stuff
		
		physics->Update(timer.TotalTime());
		//renderer->CalculateFrameStats(timer.TotalTime());
		//renderer->Update(timer.DeltaTime());
		// Render call DrawScene();
	}
}

GameObject* CoreEngine::createGameObject()
{
	Mesh* mesh = NULL; 
	GameObject* obj = new GameObject(mesh);

	return obj;
}

GameObject* CoreEngine::createGameObject(const char* filename)
{
	Mesh* mesh = createMesh(filename);
	GameObject* obj = new GameObject(mesh);

	return obj;
}

Mesh* CoreEngine::createMesh(const char* filename)
{
	std::unordered_map<std::string, Mesh*>::iterator it = meshIndex.find(filename);
	Mesh* meshObj;
	if (it == meshIndex.end())
	{
		// The mesh was not found in the meshIndex i.e It has not been loaded already
		meshObj = new Mesh(filename, renderer->getDevice());
	}
	else
	{
		// The object was found in the meshIndex i.e the mesh has already been loaded
		meshObj = it->second;
	}
	return meshObj;

#pragma region Windows Message Processing

LRESULT CoreEngine::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	/*switch (msg)
	{
	// WM_ACTIVATE is sent when the window is activated or deactivated.
	// We pause the game when the window is deactivated and unpause it
	// when it becomes active.
	case WM_ACTIVATE:
	if (LOWORD(wParam) == WA_INACTIVE)
	{
	gamePaused = true;
	timer.Stop();
	}
	else
	{
	gamePaused = false;
	timer.Start();
	}
	return 0;

	// WM_SIZE is sent when the user resizes the window.
	case WM_SIZE:
	// Save the new client area dimensions.
	windowWidth = LOWORD(lParam);
	windowHeight = HIWORD(lParam);
	if (device)
	{
	if (wParam == SIZE_MINIMIZED)
	{
	gamePaused = true;
	minimized = true;
	maximized = false;
	}
	else if (wParam == SIZE_MAXIMIZED)
	{
	gamePaused = false;
	minimized = false;
	maximized = true;
	OnResize();
	}
	else if (wParam == SIZE_RESTORED)
	{
	// Restoring from minimized state?
	if (minimized)
	{
	gamePaused = false;
	minimized = false;
	OnResize();
	}

	// Restoring from maximized state?
	else if (maximized)
	{
	gamePaused = false;
	maximized = false;
	OnResize();
	}
	else if (resizing)
	{
	// If user is dragging the resize bars, we do not resize
	// the buffers here because as the user continuously
	// drags the resize bars, a stream of WM_SIZE messages are
	// sent to the window, and it would be pointless (and slow)
	// to resize for each WM_SIZE message received from dragging
	// the resize bars.  So instead, we reset after the user is
	// done resizing the window and releases the resize bars, which
	// sends a WM_EXITSIZEMOVE message.
	}
	else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
	{
	OnResize();
	}
	}
	}
	return 0;

	// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
	gamePaused = true;
	resizing = true;
	timer.Stop();
	return 0;

	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
	gamePaused = false;
	resizing = false;
	timer.Start();
	OnResize();
	return 0;

	// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
	PostQuitMessage(0);
	return 0;

	// The WM_MENUCHAR message is sent when a menu is active and the user presses
	// a key that does not correspond to any mnemonic or accelerator key.
	case WM_MENUCHAR:
	// Don't beep when we alt-enter.
	return MAKELRESULT(0, MNC_CLOSE);

	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
	((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
	((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
	return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	return 0;
	case WM_MOUSEMOVE:
	OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	return 0;
	}*/

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

#pragma endregion
