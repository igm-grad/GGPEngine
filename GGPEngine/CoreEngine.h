#pragma once

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

#include <Windows.h>
#include <time.h>
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

	GameObject*		CreateGameObject();
	GameObject*		CreateGameObject(const char* filename);
	GameObject*		Sphere();
	GameObject*		Cube();
	GameObject*		Cone();
	GameObject*		Cylinder();
	GameObject*		Helix();
	GameObject*		Torus();

	Mesh*			CreateMesh(const char* filename);
	
	Material*       BasicMaterial();
	Material*		DiffuseMaterial();
	Material*		DiffuseNormalMaterial();

	Material*		CreateMaterial(LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile);

	DirectionalLight*	CreateDirectionalLight(XMFLOAT4& ambientColor, XMFLOAT4& diffuseColor, XMFLOAT3& direction);
	PointLight*			CreatePointLight(XMFLOAT4& ambientColor, XMFLOAT4& diffuseColor, XMFLOAT3& position);
	SpotLight*			CreateSpotLight(XMFLOAT4& ambientColor, XMFLOAT4& diffuseColor, XMFLOAT3& direction, XMFLOAT3& position);

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); // Must be public

private:
	MSG msg;
};

