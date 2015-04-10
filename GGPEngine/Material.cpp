#include "Material.h"
#include <d3dcompiler.h>
#include <direct.h>
#include <WICTextureLoader.h>

#define GetCurrentDir _getcwd

using namespace DirectX;

Material::Material(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile) : device(device), deviceContext(deviceContext)
{
	SetVertexShader(device, deviceContext, vertexShaderFile);
	SetPixelShader(device, deviceContext, pixelShaderFile);
}

Material::Material()
{
	sVertexShader = __nullptr;
	sPixelShader = __nullptr;
}


Material::~Material()
{
	deviceContext->Release();
	device->Release();
	delete sVertexShader;
	delete sPixelShader;
}

void Material::SetVertexShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR vertexShaderFile)
{
	sVertexShader = new SimpleVertexShader(device, deviceContext);
	sVertexShader->LoadShaderFile(vertexShaderFile);
}

void Material::SetVertexShader(SimpleVertexShader* simpleVertexShader)
{
	sVertexShader = simpleVertexShader;
}

void Material::SetPixelShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR pixelShaderFile)
{
	sPixelShader = new SimplePixelShader(device, deviceContext);
	sPixelShader->LoadShaderFile(pixelShaderFile);
}

void Material::SetPixelShader(SimplePixelShader* simplePixelShader)
{
	sPixelShader = simplePixelShader;
}

void Material::SetResource(const wchar_t* filename, const char* name)
{
	if (resourceMap.find(name) != resourceMap.end()) {
		resourceMap[name]->Release();
	}

	ID3D11ShaderResourceView* shaderResourceView;

	CreateWICTextureFromFile(device, deviceContext, filename, 0, &shaderResourceView);
	resourceMap[name] = shaderResourceView;
}

void Material::CreateTexture2D(int width, int height, const char* name)
{
	if (auxResourceMap.find(name) != auxResourceMap.end()) {
		auxResourceMap[name]->Release();
	}

	ID3D11Texture2D* r_buffer;
	device->CreateTexture2D(&CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_B8G8R8A8_UNORM, width, height, 1, 1), nullptr, &r_buffer);
	auxResourceMap[name] = r_buffer;


	if (resourceMap.find(name) != resourceMap.end()) {
		resourceMap[name]->Release();
	}

	ID3D11ShaderResourceView* shaderResourceView;
	device->CreateShaderResourceView(r_buffer, nullptr, &shaderResourceView);
	resourceMap[name] = shaderResourceView;
}

void Material::UpdateResourceFromBuffer(const unsigned char* buffer, CD3D11_BOX * box, int srcRowSpan, const char* name)
{
	if (auxResourceMap.find(name) == auxResourceMap.end()) {
		return;
	}

	deviceContext->UpdateSubresource(auxResourceMap[name], 0, box, buffer, srcRowSpan, 0);
}

void Material::SetSampler(const char* name)
{
	if (samplerMap.find(name) != samplerMap.end()) {
		samplerMap[name]->Release();
	}

	// TO DO: Instantiating a basic sampler until we determine abstraction for D3D11_SAMPLER_DESC
	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC samplerDescription;
	ZeroMemory(&samplerDescription, sizeof(D3D11_SAMPLER_DESC));
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDescription, &sampler);
	samplerMap[name] = sampler;
}

void Material::UpdatePixelShaderResources()
{
	for (std::map<const char*, ID3D11ShaderResourceView*>::iterator iter = resourceMap.begin(); iter != resourceMap.end(); iter++)
	{
		sPixelShader->SetShaderResourceView(iter->first, iter->second);
	}
}

void Material::UpdatePixelShaderSamplers()
{
	for (std::map<const char*, ID3D11SamplerState*>::iterator iter = samplerMap.begin(); iter != samplerMap.end(); iter++)
	{
		sPixelShader->SetSamplerState(iter->first, iter->second);
	}
}