#pragma once
#include <d3d11.h>
#include "dxerr.h"
#include <string>
#include <assert.h>

class RenderEngine
{
public:

	// Window handles and such
	HINSTANCE hAppInst;
	HWND      hMainWnd;

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
	RenderEngine();
	~RenderEngine();

	void CalculateFrameStats(float totalTime);
	void Update(float deltaTime);
};

