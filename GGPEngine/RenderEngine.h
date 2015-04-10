#pragma once
#pragma warning( disable: 4251 )
#include <d3d11.h>
#include "dxerr.h"
#include <string>
#include <assert.h>
#include "Material.h"
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

class  RenderEngine
{
protected:
	RenderEngine(HINSTANCE hInstance, WNDPROC MainWndProc);
	~RenderEngine();

	bool Initialize();
	void OnResize();
	void CalculateFrameStats(float totalTime);
	void Update(float deltaTime, std::vector<GameObject*> list);
	
	Mesh*				CreateMesh(const char* filename);
	Material*			CreateMaterial(LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile);

	DirectionalLight*	CreateDirectionalLight();
	PointLight*			CreatePointLight();
	SpotLight*			CreateSpotLight();

	Camera*				CreateCamera();

	UI* ui;

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
	D3D11_VIEWPORT viewport;
	D3D_DRIVER_TYPE driverType;
	D3D_FEATURE_LEVEL featureLevel;

	//Blend for UI
	ID3D11BlendState* blendState;

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

	bool InitMainWindow();
	bool InitDirect3D();

	friend class UI;
	friend class CoreEngine;
};

