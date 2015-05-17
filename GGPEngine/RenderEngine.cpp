#include "RenderEngine.h"
#include "UI.h"
#include <WindowsX.h>
#include <sstream>
#include <cmath>
#include <algorithm>

RenderEngine::RenderEngine(HINSTANCE hInstance, WNDPROC MainWndProc) :
	hAppInst(hInstance),
	windowCaption(L"DirectX Game"),
	driverType(D3D_DRIVER_TYPE_HARDWARE),
	windowWidth(800),
	windowHeight(600),
	enable4xMsaa(false),
	hMainWnd(0),
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
	defaultCamera->transform = new Transform();
	defaultCamera->transform->position = { 0, 0, -10 };
	defaultCamera->UpdateProjection(0.25f * 3.1415926535f, AspectRatio(), 0.1f, 100.0f);

	ZeroMemory(&defaultrasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	defaultrasterizerDesc.CullMode = D3D11_CULL_FRONT;
	defaultrasterizerDesc.FrontCounterClockwise = true;
	defaultrasterizerDesc.FillMode = D3D11_FILL_SOLID;
}

RenderEngine::~RenderEngine()
{
	// Release the DX stuff
	ReleaseMacro(renderTargetView);
	ReleaseMacro(depthStencilView);
	ReleaseMacro(swapChain);
	ReleaseMacro(depthStencilBuffer);

	ReleaseMacro(rasterizerState);
	ReleaseMacro(DSLessEqual);
	ReleaseMacro(blendState);

	// Restore default device settings
	if (deviceContext) {
		deviceContext->ClearState();
	}

	delete ui;
	delete defaultCamera;

	// Release the context and finally the device
	ReleaseMacro(deviceContext);
	ReleaseMacro(device);
}

bool RenderEngine::Initialize()
{
	if (!InitMainWindow()) {
		return false;
	}
	
	if (!InitDirect3D()) {
		return false;
	}

	return true;
}


Camera*	RenderEngine::getDefaultCamera()
{
	return	defaultCamera;
}


#pragma region Window Resizing Public

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

	if (ui)
		ui->forceReload = true;
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

void RenderEngine::UpdateScene(GameObject** gameObjects, int gameObjectsCount, double deltaTime)
{
	for (int i = 0; i < gameObjectsCount; i++) {
		if (gameObjects[i]->behavior) {
			gameObjects[i]->behavior->renderCallback(*gameObjects[i], deltaTime);
		}
	}
}

void RenderEngine::DrawScene(GameObject** gameObjects, int gameObjectsCount, double deltaTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	//Clear the renderTarget Buffer
	deviceContext->ClearRenderTargetView(renderTargetView, color);

	drawSkyBoxes();

	//Setting the rasterizer mode back to default.		
	ReleaseMacro(rasterizerState);
	device->CreateRasterizerState(&defaultrasterizerDesc, &rasterizerState);
	deviceContext->RSSetState(rasterizerState);

	// Clear the Depth buffer (erases depthbuffer)
	//  - Do this once per frame
	//  - At the beginning (before drawing anything)
	deviceContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Alpha Blending (UI needs this)
	D3D11_BLEND_DESC blendDesc;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0] = {
		true,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,
		D3D11_BLEND_ZERO,
		D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL
	};

	ReleaseMacro(blendState);
	device->CreateBlendState(&blendDesc, &blendState);

	deviceContext->OMSetBlendState(blendState, nullptr, ~0);

	// Update Camera
	defaultCamera->Update();

	// List that will hold objects to be drawn
	//std::vector<GameObject*> renderList;
	GameObject** renderList;

	// For now use default Camera
	renderList = CullGameObjectsFromCamera(defaultCamera, gameObjects, gameObjectsCount);


	//if (defaultCamera->CubeMap != nullptr)
	//{
	//	renderList[renderListCount] = defaultCamera->CubeMap;
	//	renderListCount++;							// We also have to draw one extra cube, the skyBox
	//}
	//renderlist[count] = skybox; count++
	for (int i = 0; i < renderListCount ; ++i) {
		// Set up the input assembler
		//  - These technically don't need to be set every frame, unless you're changing the
		//    input layout (different kinds of vertices) or the topology (different primitives)
		//    between draws
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Copy CPU-side data to a single CPU-side structure
		//  - Allows us to send the data to the GPU buffer in one step
		//  - Do this PER OBJECT, before drawing it
		XMFLOAT4X4 world;
		XMStoreFloat4x4(&world, XMMatrixTranspose(renderList[i]->transform->getWorldTransform()));
		renderList[i]->material->sVertexShader->SetMatrix4x4("world", world);
		renderList[i]->material->sVertexShader->SetMatrix4x4("view", defaultCamera->view);
		renderList[i]->material->sVertexShader->SetMatrix4x4("projection", defaultCamera->projection);
		renderList[i]->material->UpdateVertexShaderResources();
		renderList[i]->material->UpdateVertexShaderSamplers();
		renderList[i]->material->sVertexShader->SetData("time", &renderList[i]->material->time, sizeof(double));
		renderList[i]->material->sVertexShader->SetShader();

		// TO DO: This is gross. Less branching would be optimal since lights are the same for every object currently.
		if (directionLights.size() > 0) {
			renderList[i]->material->sPixelShader->SetData("directionalLights", &directionLights[0], sizeof(DirectionalLight) * directionLights.size());
		}

		if (pointLights.size() > 0) {
			renderList[i]->material->sPixelShader->SetData("pointLights", &pointLights[0], sizeof(PointLight) * pointLights.size());
		}

		if (spotLights.size() > 0) {
			renderList[i]->material->sPixelShader->SetData("spotLights", &spotLights[0], sizeof(SpotLight) * spotLights.size());
		}
		renderList[i]->material->sPixelShader->SetFloat3("eyePosition", defaultCamera->transform->position);
		renderList[i]->material->sPixelShader->SetFloat("specularExponent", renderList[i]->material->specularExponent);
		renderList[i]->material->sPixelShader->SetData("time", &renderList[i]->material->time, sizeof(double));

		renderList[i]->material->UpdatePixelShaderResources();
		renderList[i]->material->UpdatePixelShaderSamplers();
		renderList[i]->material->sPixelShader->SetShader();

		// Set buffers in the input assembler
		//  - This should be done PER OBJECT you intend to draw, as each object could
		//    potentially have different geometry (and therefore different buffers!)
		//  - You must have both a vertex and index buffer set to draw
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, renderList[i]->mesh->GetVertexBuffer(), &stride, &offset);
		deviceContext->IASetIndexBuffer(renderList[i]->mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);


		// Finally do the actual drawing
		//  - This should be done PER OBJECT you index to draw
		//  - This will use all of the currently set DirectX stuff (shaders, buffers, etc)
		deviceContext->DrawIndexed(
			renderList[i]->mesh->indexCount,	// The number of indices we're using in this draw
			0,
			0);
	}

	if (ui) {
		ui->Update(); // Maybe now as frequently?
		ui->Draw();
	}

	// Present the buffer
	//  - Puts the stuff on the screen
	//  - Do this EXACTLY once per frame
	//  - Always at the end of the frame
	HR(swapChain->Present(0, 0));
}

void RenderEngine::drawSkyBoxes()
{
	//Set the rasterizer to cull the front face
	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FrontCounterClockwise = true;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	ReleaseMacro(rasterizerState);
	device->CreateRasterizerState(&rastDesc, &rasterizerState);
	deviceContext->RSSetState(rasterizerState);

	//Set the depth as far as possible. and set the depth as less equal 
	//so that even the object at the farthest distance are considered closer than the box.
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	ReleaseMacro(DSLessEqual);
	device->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (defaultCamera->CubeMap != nullptr)
	{
		// Copy CPU-side data to a single CPU-side structure
		//  - Allows us to send the data to the GPU buffer in one step
		//  - Do this PER OBJECT, before drawing it
		XMFLOAT4X4 world;
		XMStoreFloat4x4(&world, XMMatrixTranspose(defaultCamera->CubeMap->transform->getWorldTransform()));
		defaultCamera->CubeMap->material->sVertexShader->SetMatrix4x4("world", world);
		defaultCamera->CubeMap->material->sVertexShader->SetMatrix4x4("view", defaultCamera->view);
		defaultCamera->CubeMap->material->sVertexShader->SetMatrix4x4("projection", defaultCamera->projection);
		defaultCamera->CubeMap->material->sVertexShader->SetShader();

		defaultCamera->CubeMap->material->UpdatePixelShaderResources();
		defaultCamera->CubeMap->material->UpdatePixelShaderSamplers();
		defaultCamera->CubeMap->material->sPixelShader->SetShader();

		// Set buffers in the input assembler
		//  - This should be done PER OBJECT you intend to draw, as each object could
		//    potentially have different geometry (and therefore different buffers!)
		//  - You must have both a vertex and index buffer set to draw
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, defaultCamera->CubeMap->mesh->GetVertexBuffer(), &stride, &offset);
		deviceContext->IASetIndexBuffer(defaultCamera->CubeMap->mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);


		// Finally do the actual drawing
		//  - This should be done PER OBJECT you index to draw
		//  - This will use all of the currently set DirectX stuff (shaders, buffers, etc)
		deviceContext->DrawIndexed(
			defaultCamera->CubeMap->mesh->indexCount,	// The number of indices we're using in this draw
			0,
			0);

	}

	//Draw the skyBoxes for every camera
	vector<Camera>::iterator it;
	for (it = cameras.begin(); it != cameras.end(); it++)
	{
		// Copy CPU-side data to a single CPU-side structure
		//  - Allows us to send the data to the GPU buffer in one step
		//  - Do this PER OBJECT, before drawing it
		XMFLOAT4X4 world;
		XMStoreFloat4x4(&world, XMMatrixTranspose((*it).CubeMap->transform->getWorldTransform()));
		(*it).CubeMap->material->sVertexShader->SetMatrix4x4("world", world);
		(*it).CubeMap->material->sVertexShader->SetMatrix4x4("view", defaultCamera->view);
		(*it).CubeMap->material->sVertexShader->SetMatrix4x4("projection", defaultCamera->projection);
		(*it).CubeMap->material->sVertexShader->SetShader();

		(*it).CubeMap->material->UpdatePixelShaderResources();
		(*it).CubeMap->material->UpdatePixelShaderSamplers();
		(*it).CubeMap->material->sPixelShader->SetShader();

		// Set buffers in the input assembler
		//  - This should be done PER OBJECT you intend to draw, as each object could
		//    potentially have different geometry (and therefore different buffers!)
		//  - You must have both a vertex and index buffer set to draw
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, (*it).CubeMap->mesh->GetVertexBuffer(), &stride, &offset);
		deviceContext->IASetIndexBuffer((*it).CubeMap->mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);


		// Finally do the actual drawing
		//  - This should be done PER OBJECT you index to draw
		//  - This will use all of the currently set DirectX stuff (shaders, buffers, etc)
		deviceContext->DrawIndexed(
			(*it).CubeMap->mesh->indexCount,	// The number of indices we're using in this draw
			0,
			0);
	}
}

Mesh* RenderEngine::CreateMesh(const char* filename)
{
	return new Mesh(filename, device);
}

//#MyChanges
// Creates a plane mesh given a width and a depth. Vertex desity must be set by setting how many vertexes per width and depth must be used.
// Minimum number of vertex per axis is 2, otherwise no triangles can be drawn.
Mesh* RenderEngine::CreatePlaneMesh(float width, int vertexPerWidth, float depth, int vertexPerDepth)
{
	// Test for minimum number of vertex
	if (vertexPerWidth < 2) vertexPerWidth = 2;
	if (vertexPerDepth < 2) vertexPerDepth = 2;
	
	std::vector<Vertex> verts;           // Verts we're assembling
	std::vector<UINT> indices;           // Indices of these verts

	// The distance between vertexes in a given axis:
	float widthStep = width / vertexPerWidth;
	float depthStep = depth / vertexPerDepth;

	float planeWidthDesloc = width / 2;
	float planeDepthDesloc = depth / 2;

		// Clear both vectors. Not necesasry, but a good practice.
	verts.clear();
	indices.clear();

	Vertex CurVertex;
	UINT IndicesIndex = 0;
		
	// Loop though the columns (z-axis)
	for (float k = 0; k < vertexPerDepth -1; k++)
	{
		// Loop though the lines (x-axis)
		for (float i = 0; i < vertexPerWidth -1; i++) // May need to change to vertexperwidth.
		{	// Creates a quad. Using two triangles
			// Vertices Position = (vertex position) - (plane dislocation)
			
			//Top Triangle
			// #1
			CurVertex.Position			= XMFLOAT3(i*widthStep - planeWidthDesloc,			0,	k*depthStep - planeDepthDesloc);
			CurVertex.Normal			= XMFLOAT3(0, 1, 0);
			CurVertex.UV				= XMFLOAT2((float)(i / vertexPerWidth - 1), (float)(k / vertexPerDepth - 1));
			verts.push_back(CurVertex);
			indices.push_back(IndicesIndex++);
				
			// #2
			CurVertex.Position			= XMFLOAT3(i*widthStep - planeWidthDesloc,			0,	(k + 1)*depthStep - planeDepthDesloc);
			CurVertex.Normal			= XMFLOAT3(0, 1, 0);
			CurVertex.UV = XMFLOAT2((float)(i / vertexPerWidth - 1), (float)((k + 1) / vertexPerDepth - 1));
			verts.push_back(CurVertex);
			indices.push_back(IndicesIndex++);

			// #3
			CurVertex.Position			= XMFLOAT3((i + 1)*widthStep - planeWidthDesloc,	0,	k*depthStep - planeDepthDesloc);
			CurVertex.Normal			= XMFLOAT3(0, 1, 0);
			CurVertex.UV = XMFLOAT2((float)((i + 1) / vertexPerWidth - 1), (float)(k / vertexPerDepth - 1));
			verts.push_back(CurVertex);
			indices.push_back(IndicesIndex++);

			//Bottom Triangle
			// #1
			CurVertex.Position			= XMFLOAT3((i+1)*widthStep - planeWidthDesloc,		0,	k*depthStep - planeDepthDesloc);
			CurVertex.Normal			= XMFLOAT3(0, 1, 0);
			CurVertex.UV = XMFLOAT2((float)((i + 1) / vertexPerWidth - 1), (float)(k / vertexPerDepth - 1));
			verts.push_back(CurVertex);
			indices.push_back(IndicesIndex++);

			// #2
			CurVertex.Position			= XMFLOAT3(i*widthStep - planeWidthDesloc,			0,	(k + 1)*depthStep - planeDepthDesloc);
			CurVertex.Normal			= XMFLOAT3(0, 1, 0);
			CurVertex.UV = XMFLOAT2((float)(i / vertexPerWidth - 1), (float)((k + 1) / vertexPerDepth - 1));
			verts.push_back(CurVertex);
			indices.push_back(IndicesIndex++);

			// #3
			CurVertex.Position			= XMFLOAT3((i + 1)*widthStep - planeWidthDesloc,	0,	(k + 1)*depthStep - (planeDepthDesloc));
			CurVertex.Normal			= XMFLOAT3(0, 1, 0);
			CurVertex.UV = XMFLOAT2((float)((i + 1) / vertexPerWidth - 1), (float)((k + 1) / vertexPerDepth - 1));
			verts.push_back(CurVertex);
			indices.push_back(IndicesIndex++);				
		}
	}

	Mesh* returnMesh = new Mesh(&verts[0], verts.size(), &indices[0], indices.size(), device);
	return returnMesh;
}

Material* RenderEngine::CreateMaterial(LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile)
{
	return new Material(device, deviceContext, vertexShaderFile, pixelShaderFile);
}

DirectionalLight* RenderEngine::CreateDirectionalLight()
{
	directionLights.push_back(DirectionalLight());
	return &directionLights.back();
}

PointLight*	RenderEngine::CreatePointLight()
{
	pointLights.push_back(PointLight());
	return &pointLights.back();
}

SpotLight*	RenderEngine::CreateSpotLight()
{
	spotLights.push_back(SpotLight());
	return &spotLights.back();
}

Camera* RenderEngine::CreateCamera()
{
	cameras.push_back(Camera());
	return &cameras.back();
}

void RenderEngine::setCameraCubeMap(Camera* camera, const wchar_t* filename)
{
	Mesh* mesh = CreateMesh("Models\\cube.obj");
	GameObject* cube = new GameObject(mesh);

	Material* cubeMapTex = CreateMaterial(L"SkyBoxVertexShader.cso", L"SkyBoxPixelShader.cso");
	cubeMapTex->SetTextureCubeResource(filename, "skyBoxTexture");
	cubeMapTex->SetClampSampler("skyBoxSampler");
	cube->material = cubeMapTex;

	camera->createCubeMap(cube);
}

float RenderEngine::getAngle(float ax, float ay, float bx, float by)
{
	// manual dot product between position vector and forward direction
	float numerator = (ax * bx) + (ay * by);

	// manual magnitude of position vector and forward direction
	float positionMagnitude = sqrt(ax * ax + ay * ay);
	float forwardDirectionMagnitude = sqrt(bx * bx + by * by);

	float denominator = positionMagnitude * forwardDirectionMagnitude;

	// calculate angle
	float theta = acos(numerator / denominator);
	return theta;
}

GameObject** RenderEngine::CullGameObjectsFromCamera(Camera* camera, GameObject** list, int listCount)
{
	// UI
	RendererDebug("TotalListCount: " + std::to_string(listCount), 0);

	// List of distance of all gameobjects from camera
	float* distFromCamera = new float[listCount];

	// Game Objects which will be within far plane
	GameObject** culledList = nullptr;
	culledList = new GameObject*[listCount];
	int culledListcount = 0;

	float sqdistance = 0.f;
	float sqfarplane = camera->farPlane * camera->farPlane;
	// First eliminate objects outside of far plane
	for (int i = 0; i < listCount; ++i)
	{
		// use distance formula to find out if objects lie outside far plane
		sqdistance = pow(list[i]->transform->position.x - camera->transform->position.x, 2) + pow(list[i]->transform->position.y - camera->transform->position.y, 2) + pow(list[i]->transform->position.z - camera->transform->position.z, 2);
		if (sqdistance < sqfarplane)
		{
			distFromCamera[i] = sqdistance;
			culledList[i] = list[i];
			++culledListcount;
		}
	}

	// Game Objects which will be whithin horizontal and vertical FOV
	GameObject** RenderList = nullptr;
	RenderList = new GameObject*[culledListcount+1];							// The additional +1 is to draw the skyBox
	int renderlistCount = 0;

	float* renderDistFromCamera = new float[culledListcount];			

	// Horizontal FOV in radians
	float HorizontalFOV = atan(AspectRatio()) / 2;

	// Vertical FOV in radians
	float VerticalFOV = camera->fov / 2;

	// Iterate through all Game Obejcts in culled list
	// Find Position Vector of Game Objects from camera. Positionvector = (GameObjectPosition - CameraPosition)
	// If Coz Inverse of (PositionVector dot Forward / |Position| * |Forward|) < FOV / 2, Game Object is in view. 
	int i, j;
	for (i = 0, j = 0; i < culledListcount; ++i)
	{
		// get Position Vector
		XMVECTOR GameObjectPosition = XMLoadFloat3(&culledList[i]->transform->position);
		XMVECTOR CameraPosition = XMLoadFloat3(&camera->transform->position);

		XMVECTOR PositionVector = GameObjectPosition - CameraPosition;

		// normalize PositionVector
		PositionVector = XMVector3Normalize(PositionVector);

		XMFLOAT3 positionVector;
		XMStoreFloat3(&positionVector, PositionVector);

		// get object angle in horizontal plane (x-z)
		float horizontalAngle = getAngle(positionVector.x, positionVector.z, camera->transform->forward.x, camera->transform->forward.z);

		// get object angle in vertical plane (y-z)
		float verticalAngle = getAngle(positionVector.y, positionVector.z, camera->transform->forward.y, camera->transform->forward.z);

		if (horizontalAngle < HorizontalFOV && verticalAngle < VerticalFOV)
		{
			renderDistFromCamera[j] = distFromCamera[i];
			RenderList[j] = culledList[i];
			++renderlistCount;
			++j;
		}
	}

	// sort Render list. For now std::sort to get quick results.
	RenderList = sortList(RenderList, renderlistCount, renderDistFromCamera);

	// return the list which needs to be drawn
	renderListCount = renderlistCount;

	// UI
	RendererDebug("renderListCount: " + std::to_string(renderListCount), 1);
	RendererDebug("First in list: " + std::to_string((int)RenderList[0]), 2);
	return RenderList;
}

GameObject** RenderEngine::sortList(GameObject** RenderList, int renderlistCount, float* renderDistFromCamera)
{
	// for now using quick sort (integer only). Will implement radix sort with floating point later.
	GameObject** sortedList = new GameObject*[renderlistCount];

	// max possible dist value 100^2 = 10000
	//unsigned int* count = new unsigned int[10001];
	unsigned int count[10001] = { 0 };

	// count for occurences of distance values in integer
	for (int i = 0; i < renderlistCount; ++i)
	{
		++count[(int)renderDistFromCamera[i]];
	}

	for (int i = 1; i < 10001; ++i)
	{
		count[i] += count[i - 1];
	}

	for (int i = 0; i < renderlistCount; ++i)
	{
		sortedList[count[(int)renderDistFromCamera[i]] - 1] = RenderList[i];
		--count[(int)renderDistFromCamera[i]];
	}

	return sortedList;
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

bool RenderEngine::InitUI(const char* url) {
	ui = new UI(this);

	ui->SetURL(url);

	if (!ui->Initialize()) {
		return false;
	}
	return true;
}

bool RenderEngine::UIExecuteJavascript(std::string javascript) {
	if (!ui) return false;
	return ui->ExecuteJavascript(javascript);
}

bool RenderEngine::UIRegisterJavascriptFunction(std::string functionName, JSFunctionCallback functionPointer) {
	if (!ui) return false;
	return ui->RegisterJavascriptFunction(functionName, functionPointer);
}

void RenderEngine::RendererDebug(std::string str, int debugLine) {
	if (ui && isDebugging) {
		std::string javascriptStr = std::string("$('#inner-debug" + std::to_string(debugLine) + "').html('" + str + "'); ");
		ui->ExecuteJavascript(javascriptStr);
	}
}

#pragma region Window Resizing Private

void RenderEngine::wmSizeHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool *gamePaused) {
	// Save the new client area dimensions.
	windowWidth = LOWORD(lParam);
	windowHeight = HIWORD(lParam);
	if (device)
	{
		if (wParam == SIZE_MINIMIZED)
		{
			*gamePaused = true;
			minimized = true;
			maximized = false;
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			*gamePaused = false;
			minimized = false;
			maximized = true;
			OnResize();
		}
		else if (wParam == SIZE_RESTORED)
		{
			// Restoring from minimized state?
			if (minimized)
			{
				gamePaused = false;
				minimized = false;
				OnResize();
			}

			// Restoring from maximized state?
			else if (maximized)
			{
				gamePaused = false;
				maximized = false;
				OnResize();
			}
			else if (resizing)
			{
				// If user is dragging the resize bars, we do not resize
				// the buffers here because as the user continuously
				// drags the resize bars, a stream of WM_SIZE messages are
				// sent to the window, and it would be pointless (and slow)
				// to resize for each WM_SIZE message received from dragging
				// the resize bars.  So instead, we reset after the user is
				// done resizing the window and releases the resize bars, which
				// sends a WM_EXITSIZEMOVE message.
			}
			else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
			{
				OnResize();
			}
		}
	}
}

void RenderEngine::wmEnterSizeMoveHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	resizing = true;
}

void RenderEngine::wmExitSizeMoveHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	resizing = false;
	OnResize();
}

// Calculates the current aspect ratio
float RenderEngine::AspectRatio() const
{
	return (float)windowWidth / windowHeight;
}

#pragma endregion

#pragma region Input Processing
bool RenderEngine::wmMouseMoveHook(WPARAM wParam, LPARAM lParam) {
	if (ui)
		return ui->wmMouseMoveHook(wParam, lParam);
	return false;
}
bool RenderEngine::wmMouseButtonDownHook(WPARAM wParam, LPARAM lParam, MouseButton btn) {
	if (ui)
		return ui->wmMouseButtonDownHook(wParam, lParam, btn);
	return false;
}
bool RenderEngine::wmMouseButtonUpHook(WPARAM wParam, LPARAM lParam, MouseButton btn) {
	if (ui)
		return ui->wmMouseButtonUpHook(wParam, lParam, btn);
	return false;
}
#pragma endregion



