#pragma once

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

#include "Windows.h"
#include "RenderEngine.h"
#include "PhysicsEngine.h"
#include "InputManager.h"
#include "GameTimer.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include <vector>
#include <unordered_map>

class GPPEngineAPI CoreEngine
{

private:
	MSG msg;

public:
	RenderEngine*				renderer;
	PhysicsEngine*				physics;
	InputManager*				input;
	GameTimer					timer;
	bool						gamePaused;
	std::unordered_map <std::string, Mesh*> meshIndex;

	std::vector<GameObject*>	gameObjects;

	CoreEngine(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd);
	CoreEngine();
	~CoreEngine();

	bool Initialize();
	void Update();
	bool exitRequested();	

	GameObject*		createGameObject();
	GameObject*		createGameObject(const char* filename);
	GameObject*		Sphere();
	GameObject*		Cube();
	GameObject*		Cone();
	GameObject*		Cylinder();
	GameObject*		Helix();
	GameObject*		Torus();

	Mesh*			createMesh(const char* filename);
	

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); // Must be public
};

