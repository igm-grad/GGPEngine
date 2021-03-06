#include "Material.h"
#include <d3dcompiler.h>
#include <direct.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#define GetCurrentDir _getcwd

#define ReleaseMacro(x) { if(x){ x->Release(); x = 0; } }


using namespace DirectX;

Material::Material(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile) : device(device), deviceContext(deviceContext)
{
	SetVertexShader(device, deviceContext, vertexShaderFile);
	SetPixelShader(device, deviceContext, pixelShaderFile);
}

Material::Material(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile, LPCWSTR geometryShaderFile) : device(device), deviceContext(deviceContext)
{
	SetVertexShader(device, deviceContext, vertexShaderFile);
	SetPixelShader(device, deviceContext, pixelShaderFile);
	SetGeometryShader(device, deviceContext, geometryShaderFile);
}

Material::Material()
{
	sVertexShader = __nullptr;
	sPixelShader = __nullptr;
}


Material::~Material()
{
	for (std::map<const char*, ID3D11ShaderResourceView*>::iterator iter = resourceMap.begin(); iter != resourceMap.end(); iter++)
	{
		ReleaseMacro(iter->second);
	}

	for (std::map<const char*, ID3D11SamplerState*>::iterator iter = samplerMap.begin(); iter != samplerMap.end(); iter++)
	{
		ReleaseMacro(iter->second);
	}

	delete sVertexShader;
	delete sPixelShader;
	delete sGeometryShader;
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
	ReleaseMacro(resourceMap[name]);

	ID3D11ShaderResourceView* shaderResourceView;

	CreateWICTextureFromFile(device, deviceContext, filename, 0, &shaderResourceView);
	resourceMap[name] = shaderResourceView;
}

void Material::SetResource(ID3D11Resource* resource, const char* name)
{
	ReleaseMacro(resourceMap[name]);

	ID3D11ShaderResourceView* shaderResourceView;

	device->CreateShaderResourceView(resource, nullptr, &shaderResourceView);
	resourceMap[name] = shaderResourceView;
}

void Material::SetGeometryShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR geometryShaderFile)
{
	sGeometryShader = new SimpleGeometryShader(device, deviceContext);
	sGeometryShader->LoadShaderFile(geometryShaderFile);
}

void Material::SetGeometryShader(SimpleGeometryShader* simpleGeometryShader)
{
	sGeometryShader = simpleGeometryShader;
}

// TO load DDS files as sky Boxes
void Material::SetTextureCubeResource(const wchar_t* filename, const char* name)
{
	ReleaseMacro(resourceMap[name]);

	ID3D11ShaderResourceView* shaderResourceView;
	//filename to be changed later
	CreateDDSTextureFromFile(device, deviceContext, filename, 0, &shaderResourceView);
	resourceMap[name] = shaderResourceView;
}


void Material::SetClampSampler(const char* name)
{
	ReleaseMacro(samplerMap[name]); 

	// TO DO: Instantiating a basic sampler until we determine abstraction for D3D11_SAMPLER_DESC
	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC samplerDescription;
	ZeroMemory(&samplerDescription, sizeof(D3D11_SAMPLER_DESC));
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDescription, &sampler);
	samplerMap[name] = sampler;
}

void Material::SetVSFloat(float data, const char* name)
{
	sVertexShader->SetFloat(name, data);
}

void Material::SetPSFloat(float data, const char* name)
{
	sPixelShader->SetFloat(name, data);
}

void Material::SetSampler(const char* name)
{
	ReleaseMacro(samplerMap[name]);
	
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

void Material::UpdateVertexShaderResources()
{
	for (std::map<const char*, ID3D11ShaderResourceView*>::iterator iter = resourceMap.begin(); iter != resourceMap.end(); iter++)
	{
		sVertexShader->SetShaderResourceView(iter->first, iter->second);
	}
}

void Material::UpdateVertexShaderSamplers()
{
	for (std::map<const char*, ID3D11SamplerState*>::iterator iter = samplerMap.begin(); iter != samplerMap.end(); iter++)
	{
		sVertexShader->SetSamplerState(iter->first, iter->second);
	}
}
