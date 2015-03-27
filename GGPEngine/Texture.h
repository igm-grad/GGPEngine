#pragma once
#include <d3d11.h>

class Texture
{
public:
	ID3D11ShaderResourceView*		shaderResourceView;
	ID3D11SamplerState*				sampler;
	
	Texture(ID3D11DeviceContext* deviceContext, ID3D11Device* device, const wchar_t* filename);
	Texture();
	~Texture();

	static void Textures(ID3D11DeviceContext* deviceContext, ID3D11Device* device, const wchar_t** filenames, int size, Texture** textures);
	void SetResource(ID3D11DeviceContext* deviceContext, ID3D11Device* device, const wchar_t* filename);
};

