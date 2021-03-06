#pragma once
#pragma warning( disable: 4251 )

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
#include "Behavior.h"
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
	std::unordered_map <std::string, Model*> modelIndex;

	std::vector<GameObject*>	gameObjects;
	std::vector<Behavior>		behaviors;

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
	GameObject*		Plane(float width, int vertexPerWidth, float depth, int vertexPerDepth);

	GameObject*		Terrain(float width, int vertexPerWidth, float depth, int vertexPerDepth);

	Model*			CreateModel(const char* filename);

	bool			LoadAnimation(GameObject* go, const char* filename);
	
	Material*       BasicMaterial();
	Material*		DiffuseMaterial();
	Material*		DiffuseNormalMaterial();
	Material*		DiffuseFluidMaterial();
	Material*		ParticleMaterial();

	// To create a skybox for a specific camera: the user has to pass a camera reference and .dds file
	void				CreateCubemap(Camera* camera, const wchar_t* filePath);					
	// To Create a skybox for the default camera. If the user does not give any camera reference, the engine Maps the skybox to the default camera 
	void				CreateCubemap(const wchar_t* filePath);									
	
	Material*			CreateMaterial(LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile);
	Material*			CreateMaterial(LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile, LPCWSTR geometryShaderFile);
	
	// Load HeightMap from a file.
	Material*			loadHeightMap(const wchar_t* filename);

	//Material*		loadHeightMap(/*const char* filename*/);
	ParticleSystem*		CreateParticleSystem(Material* particleMat, UINT maxParticles = 50);
	
	DirectionalLight*	CreateDirectionalLight(XMFLOAT4& ambientColor, XMFLOAT4& diffuseColor, XMFLOAT3& direction);
	PointLight*			CreatePointLight(XMFLOAT4& ambientColor, XMFLOAT4& diffuseColor, XMFLOAT3& position, float radius);
	SpotLight*			CreateSpotLight(XMFLOAT4& ambientColor, XMFLOAT4& diffuseColor, XMFLOAT3& direction, XMFLOAT3& position, float radius, float range);
	Camera*				CreateCamera(XMFLOAT3& position, XMFLOAT3& rotation, XMFLOAT3& forward, XMFLOAT3& up, float movementSpeed);
	Camera*				GetDefaultCamera();

	Behavior*			CreateBehavior();

	bool InitializeUI(const char* url);
	ParticleSystem* InitializeParticleSystem(Material* particleMat);
	bool UIExecuteJavascript(std::string javascript);
	bool UIRegisterJavascriptFunction(std::string functionName, JSFunctionCallback functionPointer);
	void EnableDebugLines();

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); // Must be public

private:
	MSG msg;
};

