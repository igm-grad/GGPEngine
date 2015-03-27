#pragma once
#include <d3d11.h>
#include <map>
#include "SimpleShader.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

class  Material
{
protected:
	SimpleVertexShader*		sVertexShader;
	SimplePixelShader*		sPixelShader;

	Material(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile);
	Material();
	~Material();

	void SetVertexShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR vertexShaderFile);
	void SetVertexShader(SimpleVertexShader* simpleVertexShader);
	void SetPixelShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR pixelShaderFile);
	void SetPixelShader(SimplePixelShader* simplePixelShader);

	friend class RenderEngine;
	friend class GameObject;
};

