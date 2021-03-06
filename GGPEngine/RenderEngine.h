#pragma once
#pragma warning( disable: 4251 )
#include <d3d11.h>
#include "dxerr.h"
#include <string>
#include <assert.h>
#include "Model.h"
#include <DirectXMath.h>
#include <vector>
#include "GameObject.h"
#include "Camera.h"
#include "Lighting.h"
#include "InputManager.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

// Convenience macro for releasing a COM object
#define ReleaseMacro(x) { if(x){ x->Release(); x = 0; } }

// Macro for popping up a text box based
// on a failed HRESULT and then quitting (only in debug builds)
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)												\
		{															\
		HRESULT hr = (x);										\
		if(FAILED(hr))											\
				{														\
			DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true);	\
			PostQuitMessage(0);									\
				}														\
		}														
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif

using namespace DirectX;

class UI; //Forward declaration
class ParticleSystem; //Forward declaration
typedef GPPEngineAPI void(*JSFunctionCallback)();

class  RenderEngine
{
protected:
	RenderEngine(HINSTANCE hInstance, WNDPROC MainWndProc);
	~RenderEngine();

	bool Initialize();
	void OnResize();
	void CalculateFrameStats(float totalTime);
	void UpdateScene(GameObject** gameObjects, int gameObjectsCount, double deltaTime);
	void UpdateParticleSystems(ParticleSystem** particleSystems, int particleSystemCount, double deltaTime);
	void DrawScene(GameObject** gameObjects, int gameObjectsCount, double deltaTime);
	void DrawParticleSystems(ParticleSystem** particleSystems, int particleSystemCount, double deltaTime);
	
	Mesh*									CreateMesh(const char* filename);

	Model*									CreateModel(const char* filename);
	//#MyChanges
	Mesh*									CreatePlaneMesh(float width, int vertexPerWidth, float depth, int vertexPerDepth);
	Material*								CreateMaterial(LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile);
	Material*								CreateMaterial(LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile, LPCWSTR geometryShaderFile);

	DirectionalLight*						CreateDirectionalLight();
	PointLight*								CreatePointLight();
	SpotLight*								CreateSpotLight();

	Camera*									CreateCamera();
	Camera*									getDefaultCamera();
	void									setCameraCubeMap(Camera* camera, const wchar_t* filename);

	ParticleSystem*							CreateParticleSystem(Material* mat, UINT maxParticles);

	GameObject**							CullGameObjectsFromCamera(Camera* camera, GameObject** list, int listCount);
	GameObject**							sortList(GameObject** RenderList, int renderlistCount, float* renderDistFromCamera);
	float									getAngle(float ax, float ay, float bx, float by);

	UI* ui;
	ParticleSystem* partSys;
	bool isDebugging;

private:
	// Window handles and such
	HINSTANCE hAppInst;
	HWND      hMainWnd;
	WNDPROC	  wcCallback;
	
	void wmSizeHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool *gamePaused);
	void wmEnterSizeMoveHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void wmExitSizeMoveHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool wmMouseMoveHook(WPARAM wParam, LPARAM lParam);
	bool wmMouseButtonDownHook(WPARAM wParam, LPARAM lParam, MouseButton btn);
	bool wmMouseButtonUpHook(WPARAM wParam, LPARAM lParam, MouseButton btn);
	void drawSkyBoxes();
	void AnimateModel(Model* model, float deltaTime, int animation);

	// Game and window state tracking
	bool      minimized;
	bool      maximized;
	bool      resizing;

	//Resize hooks
	float AspectRatio()const;

	// DirectX related buffers, views, etc.
	UINT msaa4xQuality;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	IDXGISwapChain* swapChain;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;
	D3D11_RASTERIZER_DESC defaultrasterizerDesc;
	ID3D11RasterizerState* rasterizerState;
	ID3D11DepthStencilState* DSLessEqual;
	D3D11_VIEWPORT viewport;
	D3D_DRIVER_TYPE driverType;
	D3D_FEATURE_LEVEL featureLevel;

	//Blend for UI
	ID3D11BlendState* blendState;

	//Additive Blending for particles
	ID3D11BlendState* additiveBlendState;

	// Derived class can set these in derived constructor to customize starting values.
	std::wstring windowCaption;
	int windowWidth;
	int windowHeight;
	bool enable4xMsaa;

	std::vector<DirectionalLight>	directionLights;
	std::vector<PointLight>			pointLights;
	std::vector<SpotLight>			spotLights;

	Camera* defaultCamera;
	std::vector<Camera> cameras;

	std::vector<ParticleSystem*>	particleSystems;

	// temp solution
	int renderListCount;

	bool InitMainWindow();
	bool InitDirect3D();

	bool InitPartSys(Material* mat);
	bool InitUI(const char* url);
	bool UIExecuteJavascript(std::string javascript);
	bool UIRegisterJavascriptFunction(std::string functionName, JSFunctionCallback functionPointer);
	void RendererDebug(std::string str, int debugLine);

	friend class UI;
	friend class ParticleSystem;
	friend class CoreEngine;
};

