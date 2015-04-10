#pragma once
	
#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>

#include <memory>
#include "Material.h"
#include "Mesh.h"
#include "RenderEngine.h"

class D3DSurfaceFactory;

class UI
{
private:
	Awesomium::WebCore* webCore;
	Awesomium::WebView* view;
	const char * url;

	int m_bossHealth;
	bool isLoading, forceReload;

	Material * material;
	Mesh * mesh;
	RenderEngine* e;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	std::unique_ptr<D3DSurfaceFactory> surfaceFactory;
	D3DSurface * surface;

	void SetView();

public:
	UI(RenderEngine* e);
	~UI();
	bool Initialize();
	void Draw();
	void Update();

	void SetURL(const char * url);

	friend class RenderEngine;
	friend class D3DSurface;

};