#include "SkyBox.h"
#include "DDSTextureLoader.h"
#include <vector>
#include "Vertex.h"

SkyBox::SkyBox()
{
}


SkyBox::~SkyBox()
{
}

struct tempVertex
{
	XMFLOAT3 Position;
};

void SkyBox::CreateCube()
{
	NumCubeVertices = 8;
	NumCubeFaces = 6;
	NumIndices = NumCubeFaces * 3 * 2;

	std::vector<tempVertex> vertices(NumCubeVertices);

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
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(tempVertex) * NumCubeVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	vertexBufferData.pSysMem = &vertices[0];

	//meith
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	//meith

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

void SkyBox::CleanUp()
{
	cubeIndexBuffer->Release();
	cubeVertBuffer->Release();

	smrv->Release();

	DSLessEqual->Release();
	
}

void SkyBox::init(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext)
{
	device = Device;
	deviceContext = DeviceContext;

	CreateCube();

	SKYMAP_VS = new SimpleVertexShader(device, deviceContext);
	SKYMAP_VS->LoadShaderFile(L"Shaders/SkyBoxVertexShader.hlsl");

	SKYMAP_PS = new SimplePixelShader(device, deviceContext);
	SKYMAP_PS->LoadShaderFile(L"Shaders/SkyBoxPixelShader.hlsl");

	CreateDDSTextureFromFile(device, deviceContext, L"Models/skyBoxSample.dds", &resource, &smrv);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	D3D11_SAMPLER_DESC sampler_desc;

	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sampler_desc, &samplerState);
}

void SkyBox::update(Transform transform)	// camera's transform
{
	camTransform = transform;

	XMMATRIX scale = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);
	XMMATRIX translation = XMMatrixTranslation(transform.position.x, transform.position.y, transform.position.z);

	XMStoreFloat4x4(&CubeWorld,(scale * translation));
}

void SkyBox::draw(XMFLOAT4X4 view, XMFLOAT4X4 projection)
{
	UINT stride = sizeof(tempVertex);
	XMFLOAT4X4 worldMatrix;

	deviceContext->OMSetDepthStencilState(DSLessEqual, 0);

	//SKYMAP_VS->SetFloat3("camPos", camTransform.position);
	SKYMAP_VS->SetMatrix4x4("world", worldMatrix);
	SKYMAP_PS->SetSamplerState("skyBoxSampler", samplerState);
	SKYMAP_PS->SetShaderResourceView("skyBoxTexture", smrv);

	SKYMAP_PS->SetShader();
	SKYMAP_VS->SetShader(true);

	deviceContext->IASetVertexBuffers(0, 1, &cubeVertBuffer, &stride, 0);
	deviceContext->IASetIndexBuffer(cubeIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->DrawIndexed(NumIndices, 0, 0);
}