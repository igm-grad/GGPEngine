#pragma once
#include <d3d11.h>
#include "dxerr.h"
#include <string>
#include <assert.h>
#include "Material.h"
#include <DirectXMath.h>
#include <vector>
#include "GameObject.h"
#include "Camera.h"

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

class  RenderEngine
{
	struct VertexShaderConstantBufferLayout
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

private:
	// Window handles and such
	HINSTANCE hAppInst;
	HWND      hMainWnd;
	WNDPROC	  wcCallback;

	float AspectRatio()const;

	// Game and window state tracking
	bool      gamePaused;
	bool      minimized;
	bool      maximized;
	bool      resizing;

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

	// Derived class can set these in derived constructor to customize starting values.
	std::wstring windowCaption;
	int windowWidth;
	int windowHeight;
	bool enable4xMsaa;

	VertexShaderConstantBufferLayout dataToSendToVSConstantBuffer;
	Material* defaultMaterial;
	Camera* defaultCamera;

	bool InitMainWindow();
	bool InitDirect3D();
	bool InitDefaultMaterial();

public:
	RenderEngine(HINSTANCE hInstance, WNDPROC MainWndProc);
	~RenderEngine();

	//Getters and setters
	ID3D11Device* getDevice() { return device; }

	bool Initialize();
	void OnResize();
	void CalculateFrameStats(float totalTime);
	void Update(float deltaTime, std::vector<GameObject*> list);


};

