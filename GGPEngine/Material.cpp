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
	
	HRESULT hr = D3DReadFileToBlob(file, &vsBlob);

	if (FAILED(hr))
	{
		MessageBox(0, L"VertexShader loading Failed", 0, 0);
		return;
	}

	// Create the shader on the device
	device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vertexShader);

	// Before cleaning up the data, create the input layout
	if (inputDescription) {
		hr = device->CreateInputLayout(
			inputDescription,					// Reference to Description
			inputDescriptionSize,				// Number of elments inside of Description
			vsBlob->GetBufferPointer(),
			vsBlob->GetBufferSize(),
			&inputLayout);

		if (FAILED(hr))
		{
			MessageBox(0, L"VertexShader loading Failed", 0, 0);
			return;
		}
	}

	// Clean up
	vsBlob->Release();
}

#include <direct.h>
#define GetCurrentDir _getcwd

void Material::SetPixelShader(ID3D11Device* device, LPCWSTR file, D3D11_INPUT_ELEMENT_DESC* inputDescription, int inputDescriptionSize)
{

	char cCurrentPath[FILENAME_MAX];

	GetCurrentDir(cCurrentPath, sizeof(cCurrentPath));

	// Load Pixel Shader ---------------------------------------
	ID3DBlob* psBlob;
	HRESULT hr = D3DReadFileToBlob(file, &psBlob);

	if (FAILED(hr))
	{
		MessageBox(0, L"PixelShader loading Failed", 0, 0);
		return;
	}

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