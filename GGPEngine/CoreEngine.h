#pragma once

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

#include "Windows.h"
#include "RenderEngine.h"
#include "PhysicsEngine.h"
#include "GameTimer.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include <vector>
#include <unordered_map>

class GPPEngineAPI CoreEngine
{
public:
	RenderEngine*				renderer;
	PhysicsEngine*				physics;
	GameTimer					timer;
	bool						gamePaused;
	std::unordered_map <std::string, Mesh*> meshIndex;

	std::vector<GameObject*>	gameObjects;

	CoreEngine(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd);
	CoreEngine();
	~CoreEngine();

	bool Initialize();
	void Update();
	
	GameObject* createGameObject();
	GameObject* createGameObject(const char* filename);
	Mesh*		createMesh(const char* filename);
	
};

