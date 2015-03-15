// ----------------------------------------------------------------------------
//  A few notes on project settings
//
//  - The project is set to use the UNICODE character set
//    - This was changed in Project Properties > Config Properties > General > Character Set
//    - This basically adds a "#define UNICODE" to the project
//
//  - The include directories were automagically correct, since the DirectX 
//    headers and libs are part of the windows SDK
//    - For instance, $(WindowsSDK_IncludePath) is set as a project include 
//      path by default.  That's where the DirectX headers are located.
//
//  - Two libraries had to be manually added to the Linker Input Dependencies
//    - d3d11.lib
//    - d3dcompiler.lib
//    - This was changed in Project Properties > Config Properties > Linker > Input > Additional Dependencies
//
//  - The Working Directory was changed to match the actual .exe's 
//    output directory, since we need to load the compiled shader files at run time
//    - This was changed in Project Properties > Config Properties > Debugging > Working Directory
//
// ----------------------------------------------------------------------------

#include <Windows.h>
#include <d3dcompiler.h>
#include "MyDemoGame.h"
#include "GameObject.h"
#include "Material.h"
#include <time.h>

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Make the game, initialize and run
	MyDemoGame game(hInstance);
	
	// If we can't initialize, we can't run
	if( !game.Init() )
		return 0;
	
	// All set to run the game
	return game.Run();
}

#pragma endregion

#pragma region Constructor / Destructor

MyDemoGame::MyDemoGame(HINSTANCE hInstance) : DirectXGame(hInstance)
{
	// Set up our custom caption and window size
	windowCaption = L"Assignment 1";
	windowWidth = 800;
	windowHeight = 600;
	// Set up camera-related matrices
	InitializeCameraMatrices();
}

MyDemoGame::~MyDemoGame()
{
	// Release all of the D3D stuff that's still hanging out
	delete gameObjects;
	delete camera;
}

#pragma endregion

#pragma region Initialization

// Initializes the base class (including the window and D3D),
// sets up our geometry and loads the shaders (among other things)
bool MyDemoGame::Init()
{
	// Make sure DirectX initializes properly
	if( !DirectXGame::Init() )
		return false;

	directionalLight = DirectionalLight();
	directionalLight.AmbientColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	directionalLight.DiffuseColor = { 0.0f, 0.0f, 1.0f, 1.0f };
	directionalLight.Direction = { -1.0f, -1.0f, 0.0f };

	pointLight = PointLight();
	pointLight.AmbientColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	pointLight.DiffuseColor = { 0.8f, 0.0f, 0.0f, 1.0f };
	pointLight.Position = {0.0f, 0.0f, 0.0f };

	// Create the necessary DirectX buffers to draw something
	CreateGeometryBuffers();

	// Load pixel & vertex shaders, and then create an input layout
	LoadShadersAndInputLayout();

	// Successfully initialized
	InputController().sharedInstance().registerKeyBinding('W', [this](){this->camera->MoveForward(); });
	InputController().sharedInstance().registerKeyBinding('S', [this](){this->camera->MoveBackward(); });
	InputController().sharedInstance().registerKeyBinding('A', [this](){this->camera->MoveLeft(); });
	InputController().sharedInstance().registerKeyBinding('D', [this](){this->camera->MoveRight(); });

	return true;
}

// Creates the vertex and index buffers for a single triangle
void MyDemoGame::CreateGeometryBuffers()
{
	Mesh* cylinder = new Mesh("sphere.obj", device);
	gameObjectsCount = 1;
	rotationAngle = 0.0f;
	gameObjects = new GameObject[gameObjectsCount];
	gameObjects[0].mesh = cylinder;
}

// Loads shaders from compiled shader object (.cso) files, and uses the
// vertex shader to create an input layout which is needed when sending
// vertex data to the device
void MyDemoGame::LoadShadersAndInputLayout()
{

	// Set up the vertex layout description
	// This has to match the vertex input layout in the vertex shader
	// We can't set up the input layout yet since we need the actual vert shader
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	0,		D3D11_INPUT_PER_VERTEX_DATA,	0},
		{"NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	12,		D3D11_INPUT_PER_VERTEX_DATA,	0},
		{"TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0,	24,		D3D11_INPUT_PER_VERTEX_DATA,	0}
	};

	// Constant buffers ----------------------------------------
	D3D11_BUFFER_DESC cBufferTransformDesc;
	cBufferTransformDesc.ByteWidth = sizeof(dataToSendToVSConstantBuffer);
	cBufferTransformDesc.Usage = D3D11_USAGE_DEFAULT;
	cBufferTransformDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferTransformDesc.CPUAccessFlags = 0;
	cBufferTransformDesc.MiscFlags = 0;
	cBufferTransformDesc.StructureByteStride = 0;

	D3D11_BUFFER_DESC cBufferDirectionalLightDesc;
	cBufferDirectionalLightDesc.ByteWidth = sizeof(directionalLight);
	cBufferDirectionalLightDesc.Usage = D3D11_USAGE_DEFAULT;
	cBufferDirectionalLightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDirectionalLightDesc.CPUAccessFlags = 0;
	cBufferDirectionalLightDesc.MiscFlags = 0;
	cBufferDirectionalLightDesc.StructureByteStride = 0;

	D3D11_BUFFER_DESC cBufferPointLightDesc;
	cBufferPointLightDesc.ByteWidth = sizeof(pointLight);
	cBufferPointLightDesc.Usage = D3D11_USAGE_DEFAULT;
	cBufferPointLightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferPointLightDesc.CPUAccessFlags = 0;
	cBufferPointLightDesc.MiscFlags = 0;
	cBufferPointLightDesc.StructureByteStride = 0;

	Material* material = new Material();
	material->SetVertexShader(device, L"VertexShader.cso", vertexDesc, 3);
	material->SetPixelShader(device, L"PixelShader.cso");
	material->SetConstantBuffer(device, &cBufferTransformDesc, "perModel");
	material->SetConstantBuffer(device, &cBufferDirectionalLightDesc, "lightBuffer");
	material->SetConstantBuffer(device, &cBufferPointLightDesc, "pointLightBuffer");
	
	for (int i = 0; i < gameObjectsCount; i++) {
		gameObjects[i].material = material;
	}
}

// Initializes the matrices necessary to represent our 3D camera
void MyDemoGame::InitializeCameraMatrices()
{
	// Create the View matrix
	// In an actual game, update this when the camera moves (every frame)
	camera = new Camera();
	camera->position = { 0, 0, -10};
	camera->UpdateProjection(0.25f * 3.1415926535f, AspectRatio(), 0.1f, 100.0f);
}

#pragma endregion

#pragma region Window Resizing

// Handles resizing the window and updating our projection matrix to match
void MyDemoGame::OnResize()
{
	// Handle base-level DX resize stuff
	DirectXGame::OnResize();

	// Update our projection matrix since the window size changed
	camera->UpdateProjection(0.25f * 3.1415926535f, AspectRatio(), 0.1f, 100.0f);
}
#pragma endregion

#pragma region Game Loop

// Update your game state
void MyDemoGame::UpdateScene(float dt)
{
	InputController().sharedInstance().executeKeyBindings();
	camera->Update();
	time_t currentTime = time(NULL);

	float radius = 2.5f;
	for (int goIndex = 0; goIndex < gameObjectsCount; goIndex++) {
		gameObjects[goIndex].position.x = radius * cos(XMConvertToRadians((rotationAngle * goIndex) + rotationAngle));
		gameObjects[goIndex].position.y = radius * sin(XMConvertToRadians((rotationAngle * goIndex) + rotationAngle));
		//gameObjects[goIndex].translateForward(cos(currentTime) * 0.0005f);
		
		//XMFLOAT3 rotationAxis = { 0.0f, 0.0f, 1.0f };
		//XMFLOAT4 quaternion = { 0.0f, 0.0f, 0.0f, 1.0f };
		//float orientation = rotationAngle + 0.1f;
		//quaternion.x = rotationAxis.x * sin(XMConvertToRadians(orientation * 0.5f));
		//quaternion.y = rotationAxis.y * sin(XMConvertToRadians(orientation * 0.5f));
		//quaternion.z = rotationAxis.z * sin(XMConvertToRadians(orientation * 0.5f));
		//quaternion.w = cos(XMConvertToRadians(orientation * 0.5f));
		//gameObjects[goIndex].rotation = quaternion;
	}

	rotationAngle += 0.01f;
	if (rotationAngle >= 360.f) {
		rotationAngle = 0.0f;
	}
}

// Clear the screen, redraw everything, present
void MyDemoGame::DrawScene()
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = {0.4f, 0.6f, 0.75f, 0.0f};

	// Clear the buffer (erases what's on the screen)
	//  - Do this once per frame
	//  - At the beginning (before drawing anything)
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(
		depthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);


	for (int goIndex = 0; goIndex < gameObjectsCount; goIndex++) {
		// Set up the input assembler
		//  - These technically don't need to be set every frame, unless you're changing the
		//    input layout (different kinds of vertices) or the topology (different primitives)
		//    between draws
		deviceContext->IASetInputLayout(gameObjects[goIndex].material->inputLayout);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set the current vertex and pixel shaders
		//  - These don't need to be set every frame YET
		//  - Once you have multiple shaders, you will need to change these
		//    between drawing objects that will use different shaders
		deviceContext->VSSetShader(gameObjects[goIndex].material->vertexShader, NULL, 0);
		deviceContext->PSSetShader(gameObjects[goIndex].material->pixelShader, NULL, 0);

		// Copy CPU-side data to a single CPU-side structure
		//  - Allows us to send the data to the GPU buffer in one step
		//  - Do this PER OBJECT, before drawing it
		XMStoreFloat4x4(&dataToSendToVSConstantBuffer.world, XMMatrixTranspose(gameObjects[goIndex].getWorldTransform()));
		dataToSendToVSConstantBuffer.view = camera->view;
		dataToSendToVSConstantBuffer.projection = camera->projection;

		// Update the GPU-side constant buffer with our single CPU-side structure
		//  - Faster than setting individual sub-variables multiple times
		//  - Do this PER OBJECT, before drawing it
		deviceContext->UpdateSubresource(
			gameObjects[goIndex].material->constantBufferMap.at("perModel"),
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
			&gameObjects[goIndex].material->constantBufferMap.at("perModel"));

		deviceContext->UpdateSubresource(
			gameObjects[goIndex].material->constantBufferMap.at("lightBuffer"),
			0,
			NULL,
			&directionalLight,
			0,
			0);

		deviceContext->PSSetConstantBuffers(
			0, 
			1, 
			&gameObjects[goIndex].material->constantBufferMap.at("lightBuffer"));

		deviceContext->UpdateSubresource(
			gameObjects[goIndex].material->constantBufferMap.at("pointLightBuffer"),
			0,
			NULL,
			&pointLight,
			0,
			0);

		deviceContext->PSSetConstantBuffers(
			1,
			1,
			&gameObjects[goIndex].material->constantBufferMap.at("pointLightBuffer"));

		// Set buffers in the input assembler
		//  - This should be done PER OBJECT you intend to draw, as each object could
		//    potentially have different geometry (and therefore different buffers!)
		//  - You must have both a vertex and index buffer set to draw
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, gameObjects[goIndex].mesh->GetVertexBuffer(), &stride, &offset);
		deviceContext->IASetIndexBuffer(gameObjects[goIndex].mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);


		// Finally do the actual drawing
		//  - This should be done PER OBJECT you index to draw
		//  - This will use all of the currently set DirectX stuff (shaders, buffers, etc)
		deviceContext->DrawIndexed(
			gameObjects[goIndex].mesh->indexCount,	// The number of indices we're using in this draw
			0,
			0);
	}

	// Present the buffer
	//  - Puts the stuff on the screen
	//  - Do this EXACTLY once per frame
	//  - Always at the end of the frame
	HR(swapChain->Present(0, 0));
}

#pragma endregion

#pragma region Mouse Input

// These methods don't do much currently, but can be used for mouse-related input

void MyDemoGame::OnMouseDown(WPARAM btnState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;

	SetCapture(hMainWnd);
}

void MyDemoGame::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void MyDemoGame::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (btnState & 1) {
		camera->RotatePitch(y - prevMousePos.y);
		camera->RotateYaw(x - prevMousePos.x);
	}
	
	prevMousePos.x = x;
	prevMousePos.y = y;
	
}
#pragma endregion