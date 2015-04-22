#pragma once
#include "SimpleShader.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Transform.h"

using namespace DirectX;

class SkyBox
{
public:
	SkyBox();
	~SkyBox();

	ID3D11DeviceContext*								deviceContext;
	ID3D11Device*										device;
	ID3D11Buffer*										cubeIndexBuffer;
	ID3D11Buffer*										cubeVertBuffer;
	SimpleVertexShader*									SKYMAP_VS;
	SimplePixelShader*									SKYMAP_PS;
	ID3D11Resource*										resource;
	ID3D11ShaderResourceView*							smrv;
	ID3D11SamplerState*									samplerState;
	ID3D11DepthStencilState*							DSLessEqual;
	int													NumCubeVertices;
	int													NumCubeFaces;
	int													NumIndices;
	XMMATRIX											CubeWorld;
	Transform											camTransform;

	void CreateCube();
	void CleanUp();
	void init(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext);
	void update(Transform transform);	// camera's transform
	void draw(XMFLOAT4X4 view, XMFLOAT4X4 projection);
};

