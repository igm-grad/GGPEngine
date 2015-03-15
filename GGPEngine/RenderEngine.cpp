#include "RenderEngine.h"
#include <WindowsX.h>
#include <sstream>

RenderEngine::RenderEngine(HINSTANCE hInstance, WNDPROC MainWndProc) :
	hAppInst(hInstance),
	windowCaption(L"DirectX Game"),
	driverType(D3D_DRIVER_TYPE_HARDWARE),
	windowWidth(800),
	windowHeight(600),
	enable4xMsaa(false),
	hMainWnd(0),
	gamePaused(false),
	minimized(false),
	maximized(false),
	resizing(false),
	msaa4xQuality(0),
	device(0),
	deviceContext(0),
	swapChain(0),
	depthStencilBuffer(0),
	renderTargetView(0),
	depthStencilView(0)
{
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	wcCallback = MainWndProc;

	defaultCamera = new Camera();
	defaultCamera->position = { 0, 0, -10 };
	defaultCamera->UpdateProjection(0.25f * 3.1415926535f, AspectRatio(), 0.1f, 100.0f);
}

RenderEngine::~RenderEngine()
{
	// Release the DX stuff
	ReleaseMacro(renderTargetView);
	ReleaseMacro(depthStencilView);
	ReleaseMacro(swapChain);
	ReleaseMacro(depthStencilBuffer);

	// Restore default device settings
	if (deviceContext)
		deviceContext->ClearState();

	// Release the context and finally the device
	ReleaseMacro(deviceContext);
	ReleaseMacro(device);
}

bool RenderEngine::Initialize()
{
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;

	if (!InitDefaultMaterial())
		return false;
	return true;
}

bool RenderEngine::InitMainWindow() {
	// Actually create the window
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = wcCallback;  // TODO: MainWndProc for message processing, Can't be a member function!  Hence our global version
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	hMainWnd = CreateWindow(L"D3DWndClassName", windowCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hAppInst, 0);
	if (!hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(hMainWnd, SW_SHOW);
	UpdateWindow(hMainWnd);

	return true;
}

bool RenderEngine::InitDirect3D() {
	UINT createDeviceFlags = 0;

	// Do we want a debug device?
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Set up a swap chain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = windowWidth;
	swapChainDesc.BufferDesc.Height = windowHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hMainWnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;
	if (enable4xMsaa)
	{
		// Set up 4x MSAA
		swapChainDesc.SampleDesc.Count = 4;
		swapChainDesc.SampleDesc.Quality = msaa4xQuality - 1;
	}
	else
	{
		// No MSAA
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}

	// Create the device and swap chain and determine the supported feature level
	featureLevel = D3D_FEATURE_LEVEL_9_1; // Will be overwritten by next line
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		0,
		driverType,
		0,
		createDeviceFlags,
		0,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapChain,
		&device,
		&featureLevel,
		&deviceContext);

	// Handle any device creation or DirectX version errors
	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed", 0, 0);
		return false;
	}

	// Check for 4X MSAA quality support
	HR(device->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		4,
		&msaa4xQuality));
	assert(msaa4xQuality > 0); // Potential problem if quality is 0

	// The remaining steps also need to happen each time the window
	// is resized, so just run the OnResize method
	OnResize();
	
	return true;
}

bool RenderEngine::InitDefaultMaterial() {

	// Set up the vertex layout description
	// This has to match the vertex input layout in the vertex shader
	// We can't set up the input layout yet since we need the actual vert shader
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Constant buffers ----------------------------------------
	D3D11_BUFFER_DESC cBufferTransformDesc;
	cBufferTransformDesc.ByteWidth = sizeof(dataToSendToVSConstantBuffer);
	cBufferTransformDesc.Usage = D3D11_USAGE_DEFAULT;
	cBufferTransformDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferTransformDesc.CPUAccessFlags = 0;
	cBufferTransformDesc.MiscFlags = 0;
	cBufferTransformDesc.StructureByteStride = 0;

	defaultMaterial = new Material();
	defaultMaterial->SetVertexShader(device, L"VertexShader.cso", vertexDesc, 3);
	defaultMaterial->SetPixelShader(device, L"PixelShader.cso");
	defaultMaterial->SetConstantBuffer(device, &cBufferTransformDesc, "perModel");

	return true;
}

#pragma region Window Resizing

// Calculates the current aspect ratio
float RenderEngine::AspectRatio() const
{
	return (float)windowWidth / windowHeight;
}

// When the window is resized, the underlying buffers (textures) must
// also be resized to match.  
void RenderEngine::OnResize()
{
	// Release the views, since we'll be destroying
	// the corresponding buffers.
	ReleaseMacro(renderTargetView);
	ReleaseMacro(depthStencilView);
	ReleaseMacro(depthStencilBuffer);

	// Resize the swap chain to match the window and
	// recreate the render target view
	HR(swapChain->ResizeBuffers(
		1,
		windowWidth,
		windowHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0));
	ID3D11Texture2D* backBuffer;
	HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(device->CreateRenderTargetView(backBuffer, 0, &renderTargetView));
	ReleaseMacro(backBuffer);

	// Set up the description of the texture to use for the
	// depth stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = windowWidth;
	depthStencilDesc.Height = windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	if (enable4xMsaa)
	{
		// Turn on 4x MultiSample Anti Aliasing
		// This must match swap chain MSAA values
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = msaa4xQuality - 1;
	}
	else
	{
		// No MSAA
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	// Create the depth/stencil buffer and corresponding view
	HR(device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer));
	HR(device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView));

	// Bind these views to the pipeline, so rendering actually
	// uses the underlying textures
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	// Update the viewport and set it on the device
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)windowWidth;
	viewport.Height = (float)windowHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	deviceContext->RSSetViewports(1, &viewport);
}
#pragma endregion

void RenderEngine::CalculateFrameStats(float totalTime)
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((totalTime - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << windowCaption << L"    "
			<< L"Width: " << windowWidth << L"    "
			<< L"Height: " << windowHeight << L"    "
			<< L"FPS: " << fps << L"    "
 			<< L"Frame Time: " << mspf << L" (ms)";

		// Include feature level
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1: outs << "    DX 11.1"; break;
		case D3D_FEATURE_LEVEL_11_0: outs << "    DX 11.0"; break;
		case D3D_FEATURE_LEVEL_10_1: outs << "    DX 10.1"; break;
		case D3D_FEATURE_LEVEL_10_0: outs << "    DX 10.0"; break;
		case D3D_FEATURE_LEVEL_9_3:  outs << "    DX 9.3";  break;
		case D3D_FEATURE_LEVEL_9_2:  outs << "    DX 9.2";  break;
		case D3D_FEATURE_LEVEL_9_1:  outs << "    DX 9.1";  break;
		default:                     outs << "    DX ???";  break;
		}

		SetWindowText(hMainWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void RenderEngine::Update(float totalTime, std::vector<GameObject*> list) {

// Background color (Cornflower Blue in this case) for clearing
const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

// Clear the buffer (erases what's on the screen)
//  - Do this once per frame
//  - At the beginning (before drawing anything)
deviceContext->ClearRenderTargetView(renderTargetView, color);
deviceContext->ClearDepthStencilView(
	depthStencilView,
	D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
	1.0f,
	0);


for (GameObject* g : list) {
	// Set up the input assembler
	//  - These technically don't need to be set every frame, unless you're changing the
	//    input layout (different kinds of vertices) or the topology (different primitives)
	//    between draws
	deviceContext->IASetInputLayout(defaultMaterial->inputLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the current vertex and pixel shaders
	//  - These don't need to be set every frame YET
	//  - Once you have multiple shaders, you will need to change these
	//    between drawing objects that will use different shaders
	deviceContext->VSSetShader(defaultMaterial->vertexShader, NULL, 0);
	deviceContext->PSSetShader(defaultMaterial->pixelShader, NULL, 0);

	// Copy CPU-side data to a single CPU-side structure
	//  - Allows us to send the data to the GPU buffer in one step
	//  - Do this PER OBJECT, before drawing it
	XMStoreFloat4x4(&dataToSendToVSConstantBuffer.world, XMMatrixTranspose(g->getWorldTransform()));
	dataToSendToVSConstantBuffer.view = defaultCamera->view;
	dataToSendToVSConstantBuffer.projection = defaultCamera->projection;

	// Update the GPU-side constant buffer with our single CPU-side structure
	//  - Faster than setting individual sub-variables multiple times
	//  - Do this PER OBJECT, before drawing it
	deviceContext->UpdateSubresource(
		defaultMaterial->constantBufferMap.at("perModel"),
		0,
		NULL,
		&dataToSendToVSConstantBuffer,
		0,
		0);

	// Set the constant buffer to be used by the Vertex Shader
	//  - This should be done PER OBJECT you intend to draw, as each object
	//    will probably have different data to send to the shader (matrices
	//    in this case)
	deviceContext->VSSetConstantBuffers(
		0,	// Corresponds to the constant buffer's register in the vertex shader
		1,
		&defaultMaterial->constantBufferMap.at("perModel"));

	// Set buffers in the input assembler
	//  - This should be done PER OBJECT you intend to draw, as each object could
	//    potentially have different geometry (and therefore different buffers!)
	//  - You must have both a vertex and index buffer set to draw
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, g->mesh->GetVertexBuffer(), &stride, &offset);
	deviceContext->IASetIndexBuffer(g->mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);


	// Finally do the actual drawing
	//  - This should be done PER OBJECT you index to draw
	//  - This will use all of the currently set DirectX stuff (shaders, buffers, etc)
	deviceContext->DrawIndexed(
		g->mesh->indexCount,	// The number of indices we're using in this draw
		0,
		0);
}

// Present the buffer
//  - Puts the stuff on the screen
//  - Do this EXACTLY once per frame
//  - Always at the end of the frame
HR(swapChain->Present(0, 0));
}