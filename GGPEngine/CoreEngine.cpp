#include "CoreEngine.h"

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
	renderer = new RenderEngine(hInstance, (WNDPROC)MainWndProc);
	physics = new PhysicsEngine();
	input = new InputManager();
	gamePaused = false;
	gCore = this;
	msg = { { 0 } };
}


CoreEngine::~CoreEngine()
{
}

bool CoreEngine::Initialize()
{
	if (renderer->Initialize() && physics->Initialize()) {
		timer.Start();
		physics->currentTime = timer.TotalTime();
		return true;
	}
	return false;
}

void CoreEngine::Update()
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
		// No message, so continue the game loop
		timer.Tick();

		if (gamePaused)
		{
			Sleep(100);
		}
		else
		{
			// This is ugly, but I'm a shitty programmer :(
			for (GameObject* gameObject : gameObjects)
			{
				if (gameObject->behavior != NULL && gameObject->behavior->keyInputMap.size() != 0)
				{
					for (std::map<KeyCode, KeyCallback>::iterator iter = gameObject->behavior->keyInputMap.begin(); iter != gameObject->behavior->keyInputMap.end(); iter++)
					{
						if (input->GetKey(iter->first)) {
							iter->second(*gameObject);
						}
					}

					for (std::map<KeyCode, KeyDownCallback>::iterator iter = gameObject->behavior->keyDownInputMap.begin(); iter != gameObject->behavior->keyDownInputMap.end(); iter++)
					{
						if (input->GetKeyDown(iter->first)) {
							iter->second(*gameObject);
						}
					}
				}
			}

			// Standard game loop type stuff
			physics->Update(timer.TotalTime());

			// There must be a logics update method
			// TODO: transfer this code to a gamoobject 
			// component update method
#pragma region Input tests
			if (input->GetKey(KEYCODE_W)) {
				auto go = gameObjects[0];
				go->transform->MoveForward();
			}
			if (input->GetKey(KEYCODE_S)) {
				auto go = gameObjects[0];
				go->transform->MoveBackward();
			}
			if (input->GetKey(KEYCODE_A)) {
				auto go = gameObjects[0];
				go->transform->MoveLeft();
			}
			if (input->GetKey(KEYCODE_D)) {
				auto go = gameObjects[0];
				go->transform->MoveRight();
			}
			if (input->GetKey(KEYCODE_C)) {
				auto go = gameObjects[0];
				go->position.y += 0.001f;
			}
			if (input->GetKey(KEYCODE_D)) {
				auto go = gameObjects[0];
				go->position.y -= 0.001f;
			}
			if (input->GetKey(KEYCODE_E)) {
				auto go = gameObjects[0];
				go->position.z += 0.001f;
			}
			if (input->GetKey(KEYCODE_F)) {
				auto go = gameObjects[0];
				go->position.z -= 0.001f;
			}
			if (input->GetKeyDown(KEYCODE_A)) {
				OutputDebugStringA("KeyDown A\n");
			}
			if (input->GetKeyUp(KEYCODE_A)) {
				OutputDebugStringA("KeyUp A\n");
			}


			//renderer->CalculateFrameStats(timer.TotalTime());
			renderer->UpdateScene(&gameObjects[0], gameObjects.size(), timer.TotalTime());
			renderer->DrawScene(&gameObjects[0], gameObjects.size(), timer.TotalTime());

			// Flush the InputManager at the end of every frame
			input->Flush();
		}
	}
}

bool CoreEngine::exitRequested() 
{
	return msg.message == WM_QUIT;
}

GameObject* CoreEngine::CreateGameObject()
{
	Mesh* mesh = NULL;
	GameObject* obj = new GameObject(mesh);
	obj->transform = new Transform();
	gameObjects.push_back(obj);
	return obj;
}

GameObject* CoreEngine::CreateGameObject(const char* filename)
{
	Mesh* mesh = CreateMesh(filename);
	GameObject* obj = new GameObject(mesh);
	obj->transform = new Transform();
	gameObjects.push_back(obj);
	return obj;
}

GameObject*	CoreEngine::Sphere()
{
	return CreateGameObject("Models\\Sphere.obj");
}

GameObject*	CoreEngine::Cube()
{
	return CreateGameObject("Models\\Cube.obj");
}

GameObject*	CoreEngine::Cone()
{
	return CreateGameObject("Models\\Cone.obj");
}

GameObject*	CoreEngine::Cylinder()
{
	return CreateGameObject("Models\\Cylinder.obj");
}

GameObject*	CoreEngine::Helix()
{
	return CreateGameObject("Models\\Helix.obj");
}

GameObject*	CoreEngine::Torus()
{
	return CreateGameObject("Models\\Torus.obj");
}

Mesh* CoreEngine::CreateMesh(const char* filename)
{
	std::unordered_map<std::string, Mesh*>::iterator it = meshIndex.find(filename);
	Mesh* meshObj;
	if (it == meshIndex.end())
	{
		// The mesh was not found in the meshIndex i.e It has not been loaded already
		meshObj = renderer->CreateMesh(filename);
		meshIndex.insert({ filename, meshObj });
	}
	else
	{
		// The object was found in the meshIndex i.e the mesh has already been loaded
		meshObj = it->second;
	}
	return meshObj;
}

Material* CoreEngine::BasicMaterial()
{
	return CreateMaterial(L"DebugVertexShader.cso", L"DebugPixelShader.cso");
}

Material* CoreEngine::DiffuseMaterial()
{
	Material* diffuseMaterial = CreateMaterial(L"DiffuseVertexShader.cso", L"DiffusePixelShader.cso");
	diffuseMaterial->SetSampler("diffuseSampler");
	return diffuseMaterial;
}

Material* CoreEngine::DiffuseNormalMaterial()
{
	Material* diffuseNormalMaterial = CreateMaterial(L"DiffuseNormalVertexShader.cso", L"DiffuseNormalPixelShader.cso");
	diffuseNormalMaterial->SetSampler("omniSampler");
	return diffuseNormalMaterial;
}

Material* CoreEngine::CreateMaterial(LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile)
{
	return renderer->CreateMaterial(vertexShaderFile, pixelShaderFile);
}

DirectionalLight* CoreEngine::CreateDirectionalLight(XMFLOAT4& ambientColor, XMFLOAT4& diffuseColor, XMFLOAT3& direction)
{
	DirectionalLight* directionalLight = renderer->CreateDirectionalLight();
	directionalLight->ambientColor = ambientColor;
	directionalLight->diffuseColor = diffuseColor;
	directionalLight->direction = direction;
	return directionalLight;
}

PointLight*	CoreEngine::CreatePointLight(XMFLOAT4& ambientColor, XMFLOAT4& diffuseColor, XMFLOAT3& position, float radius)
{
	PointLight* pointLight = renderer->CreatePointLight();
	pointLight->ambientColor = ambientColor;
	pointLight->diffuseColor = diffuseColor;
	pointLight->position = position;
	pointLight->radius = radius;
	return pointLight;
}

SpotLight* CoreEngine::CreateSpotLight(XMFLOAT4& ambientColor, XMFLOAT4& diffuseColor, XMFLOAT3& direction, XMFLOAT3& position, float radius, float range)
{
	SpotLight* spotLight = renderer->CreateSpotLight();
	spotLight->ambientColor = ambientColor;
	spotLight->diffuseColor = diffuseColor;
	spotLight->direction = direction;
	spotLight->position = position;
	spotLight->radius = radius;
	spotLight->range = range;
	return spotLight;
}

Camera* CoreEngine::CreateCamera(XMFLOAT3& position, XMFLOAT3& rotation, XMFLOAT3& forward, XMFLOAT3& up, float movementSpeed)
{
	Camera* camera = renderer->CreateCamera();
	camera->transform = new Transform();
	camera->transform->position = position;
	camera->transform->rotation = rotation;
	camera->transform->forward = forward;
	camera->transform->up = up;
	camera->transform->movementSpeed = movementSpeed;
	return camera;
}

Behavior* CoreEngine::CreateBehavior()
{
	behaviors.push_back(Behavior());
	return &behaviors.back();
}

#pragma region Windows Message Processing

LRESULT CoreEngine::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
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
		//Renderer can decide to puase the game based on that!
		renderer->wmSizeHook(hwnd, msg, wParam, lParam, &gamePaused);
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		gamePaused = true;
		renderer->wmEnterSizeMoveHook(hwnd, msg, wParam, lParam);
		timer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		gamePaused = false;
		renderer->wmExitSizeMoveHook(hwnd, msg, wParam, lParam);
		timer.Start();
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
		input->OnMouseDown(MOUSEBUTTON_LEFT, wParam, lParam);
		return 0;
	case WM_MBUTTONDOWN:
		input->OnMouseDown(MOUSEBUTTON_MIDDLE, wParam, lParam);
		return 0;
	case WM_RBUTTONDOWN:
		input->OnMouseDown(MOUSEBUTTON_RIGHT, wParam, lParam);
		return 0;
	case WM_XBUTTONDOWN:
		input->OnMouseDown(MOUSEBUTTON_X, wParam, lParam);
		return 0;
	case WM_LBUTTONUP:
		input->OnMouseUp(MOUSEBUTTON_LEFT, wParam, lParam);
		return 0;
	case WM_MBUTTONUP:
		input->OnMouseUp(MOUSEBUTTON_MIDDLE, wParam, lParam);
		return 0;
	case WM_RBUTTONUP:
		input->OnMouseUp(MOUSEBUTTON_RIGHT, wParam, lParam);
		return 0;
	case WM_XBUTTONUP:
		input->OnMouseUp(MOUSEBUTTON_X, wParam, lParam);
		return 0;
	case WM_MOUSEMOVE:
		input->OnMouseMove(wParam, lParam);
		return 0;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: // key down if alt key is presed
		input->OnKeyDown(wParam, lParam);
		return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP: // key up if alt key is pressed
		input->OnKeyUp(wParam, lParam);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

#pragma endregion
