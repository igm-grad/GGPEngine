#include "Texture.h"
#include <WICTextureLoader.h>

using namespace DirectX;

Texture::Texture(ID3D11DeviceContext* deviceContext, ID3D11Device* device, const wchar_t* filename) : shaderResourceView(0), sampler(0)
{

}

Texture::Texture() : shaderResourceView(0), sampler(0)
{

}


Texture::~Texture() 
{
	shaderResourceView->Release();
	sampler->Release();
}

void Texture::Textures(ID3D11DeviceContext* deviceContext, ID3D11Device* device, const wchar_t** filenames, int size, Texture** textures)
{
	*textures = new Texture[size];
	for (int i = 0; i < size; i++) {
		textures[i]->SetResource(deviceContext, device, filenames[i]);
	}
}

void Texture::SetResource(ID3D11DeviceContext* deviceContext, ID3D11Device* device, const wchar_t* filename)
{
	CreateWICTextureFromFile(device, deviceContext, filename, 0, &shaderResourceView);

	// TO DO: Instantiating a basic sampler until we determine abstraction for D3D11_SAMPLER_DESC
	D3D11_SAMPLER_DESC samplerDescription;
	ZeroMemory(&samplerDescription, sizeof(D3D11_SAMPLER_DESC));
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDescription, &sampler);
}