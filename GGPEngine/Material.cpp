#include "Material.h"
#include <d3dcompiler.h>
#include <direct.h>
#define GetCurrentDir _getcwd

Material::Material(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile)
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

