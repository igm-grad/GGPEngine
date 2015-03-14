#pragma once
#include <d3d11.h>
#include <map>

class Material
{
public:

	ID3D11PixelShader*		pixelShader;
	ID3D11VertexShader*		vertexShader;
	ID3D11InputLayout*      inputLayout;
	//ID3D11Buffer*			constantBuffer;
	std::map<const char*, ID3D11Buffer*> constantBufferMap;

	Material();
	~Material();

	ID3D11Buffer* GetConstantBuffer(const char* key);

	void SetVertexShader(ID3D11Device* device, LPCWSTR file, D3D11_INPUT_ELEMENT_DESC* inputDescription, int inputDescriptionSize);
	void SetPixelShader(ID3D11Device* device, LPCWSTR file, D3D11_INPUT_ELEMENT_DESC* inputDescription, int inputDescriptionSize);
	void SetPixelShader(ID3D11Device* device, LPCWSTR file);
	void SetConstantBuffer(ID3D11Device* device, D3D11_BUFFER_DESC* bufferDescription, const char* key);

};

