#include "Material.h"
#include <d3dcompiler.h>


Material::Material()
{
	vertexShader = __nullptr;
	pixelShader = __nullptr;
	inputLayout = __nullptr;
}


Material::~Material()
{
	vertexShader->Release();
	pixelShader->Release();
	inputLayout->Release();

	for (std::map<const char*, ID3D11Buffer*>::iterator iter = constantBufferMap.begin(); iter != constantBufferMap.end(); iter++) {
		iter->second->Release();
	}
}

ID3D11Buffer* Material::GetConstantBuffer(const char* key)
{
	return constantBufferMap.at(key);
}

void Material::SetVertexShader(ID3D11Device* device, LPCWSTR file, D3D11_INPUT_ELEMENT_DESC* inputDescription, int inputDescriptionSize)
{
	// Load Vertex Shader --------------------------------------
	ID3DBlob* vsBlob;
	D3DReadFileToBlob(file, &vsBlob);

	// Create the shader on the device
	device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vertexShader);

	// Before cleaning up the data, create the input layout
	if (inputDescription) {
		device->CreateInputLayout(
			inputDescription,					// Reference to Description
			inputDescriptionSize,				// Number of elments inside of Description
			vsBlob->GetBufferPointer(),
			vsBlob->GetBufferSize(),
			&inputLayout);
	}

	// Clean up
	vsBlob->Release();
}
void Material::SetPixelShader(ID3D11Device* device, LPCWSTR file, D3D11_INPUT_ELEMENT_DESC* inputDescription, int inputDescriptionSize)
{
	// Load Pixel Shader ---------------------------------------
	ID3DBlob* psBlob;
	D3DReadFileToBlob(file, &psBlob);

	// Create the shader on the device
	device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&pixelShader);

	// Clean up
	psBlob->Release();
}
void Material::SetPixelShader(ID3D11Device* device, LPCWSTR file)
{
	SetPixelShader(device, file, __nullptr, 0);
}

void Material::SetConstantBuffer(ID3D11Device* device, D3D11_BUFFER_DESC* bufferDescription, const char* key)
{
	ID3D11Buffer* constantBuffer;
	device->CreateBuffer(bufferDescription, NULL, &constantBuffer);
	constantBufferMap.insert({ key, constantBuffer });
}