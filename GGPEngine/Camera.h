#pragma once
#pragma warning( disable: 4251 )

#include <d3d11.h>
#include <DirectXMath.h>
#include "Transform.h"
#include <vector>
#include "Vertex.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

using namespace DirectX;

struct CubeMap
{
	ID3D11Buffer* cubeIndexBuffer;
	ID3D11Buffer* cubeVertBuffer;

	ID3D11VertexShader* SKYMAP_VS;
	ID3D11PixelShader* SKYMAP_PS;
	ID3D10Blob* SKYMAP_VS_Buffer;
	ID3D10Blob* SKYMAP_PS_Buffer;

	ID3D11ShaderResourceView* smrv;

	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;

	int NumCubeVertices;
	int NumCubeFaces;

	XMMATRIX CubeWorld;

	void CreateCube(ID3D11Device* device)
	{
		NumCubeVertices = 8;
		NumCubeFaces = 6;

		std::vector<Vertex> vertices(NumCubeVertices);

		vertices[0].Position.x = 1.0f;
		vertices[0].Position.y = 1.0f;
		vertices[0].Position.z = 1.0f;

		vertices[1].Position.x = -1.0f;
		vertices[1].Position.y = 1.0f;
		vertices[1].Position.z = 1.0f;

		vertices[2].Position.x = -1.0f;
		vertices[2].Position.y = -1.0f;
		vertices[2].Position.z = 1.0f;

		vertices[3].Position.x = 1.0f;
		vertices[3].Position.y = -1.0f;
		vertices[3].Position.z = 1.0f;

		vertices[4].Position.x = 1.0f;
		vertices[4].Position.y = 1.0f;
		vertices[4].Position.z = -1.0f;

		vertices[5].Position.x = -1.0f;
		vertices[5].Position.y = 1.0f;
		vertices[5].Position.z = -1.0f;

		vertices[6].Position.x = -1.0f;
		vertices[6].Position.y = -1.0f;
		vertices[6].Position.z = -1.0f;

		vertices[7].Position.x = 1.0f;
		vertices[7].Position.y = -1.0f;
		vertices[7].Position.z = -1.0f;


		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * NumCubeVertices;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = &vertices[0];
		device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &cubeVertBuffer);

		std::vector<DWORD> indices(NumCubeFaces * 3 * 2);

		//face 1	Front
		indices[0] = 0;
		indices[1] = 2;
		indices[2] = 1;
		indices[3] = 0;
		indices[4] = 3;
		indices[5] = 2;

		//face 2	Left
		indices[6] = 1;
		indices[7] = 2;
		indices[8] = 6;
		indices[9] = 1;
		indices[10] = 6;
		indices[11] = 5;

		//face 3	Right
		indices[12] = 0;
		indices[13] = 2;
		indices[14] = 1;
		indices[15] = 0;
		indices[16] = 3;
		indices[17] = 2;

		//face 4	Top
		indices[18] = 0;
		indices[19] = 5;
		indices[20] = 4;
		indices[21] = 0;
		indices[22] = 1;
		indices[23] = 5;

		//face 5	Bottom
		indices[24] = 3;
		indices[25] = 6;
		indices[26] = 7;
		indices[27] = 3;
		indices[28] = 2;
		indices[29] = 6;

		//face 6	Back
		indices[30] = 4;
		indices[31] = 6;
		indices[32] = 7;
		indices[33] = 4;
		indices[34] = 5;
		indices[35] = 6;

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * NumCubeFaces * 3 * 2;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA iinitData;

		iinitData.pSysMem = &indices[0];
		device->CreateBuffer(&indexBufferDesc, &iinitData, &cubeIndexBuffer);
	}

	void CleanUp()
	{
		cubeIndexBuffer->Release();
		cubeVertBuffer->Release();

		SKYMAP_PS->Release();
		SKYMAP_VS->Release();
		SKYMAP_PS_Buffer->Release();
		SKYMAP_VS_Buffer->Release();

		smrv->Release();

		DSLessEqual->Release();
		RSCullNone->Release();
	}
};

class  Camera
{
public:
	
	Transform*	transform;
	
	XMFLOAT4X4	view;
	XMFLOAT4X4	projection;
	float		fov;
	float		farPlane;


	Camera() {};
	~Camera();


	void UpdateProjection(float fov, float aspectRatio, float zNear, float zFar);
	void Update();

};

