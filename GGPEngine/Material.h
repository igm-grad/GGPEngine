#pragma once
#pragma warning( disable: 4251 )
#include <d3d11.h>
#include <map>
#include "SimpleShader.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

class GPPEngineAPI Material
{
public:
	void SetResource(const wchar_t* filename, const char* name);
	void SetTextureCubeResource(const wchar_t* filename, const char* name);
	void SetResource(ID3D11Resource* resource, const char* name);
	void SetSampler(const char* name);								// TO DO: We only setting a basic sampler for a key... need to think this through
	void SetClampSampler(const char* name);							//We need a clamp for Cube map
protected:
	std::map<const char*, ID3D11ShaderResourceView*>	resourceMap;
	std::map<const char*, ID3D11SamplerState*>			samplerMap;
	ID3D11DeviceContext*								deviceContext;
	ID3D11Device*										device;
	SimpleVertexShader*									sVertexShader;
	SimplePixelShader*									sPixelShader;

	Material(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile);
	Material();
	~Material();

	void SetVertexShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR vertexShaderFile);
	void SetVertexShader(SimpleVertexShader* simpleVertexShader);
	void SetPixelShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR pixelShaderFile);
	void SetPixelShader(SimplePixelShader* simplePixelShader);
	void UpdatePixelShaderResources();
	void UpdatePixelShaderSamplers();

	friend class UI;
	friend class RenderEngine;
	friend class GameObject;
};

