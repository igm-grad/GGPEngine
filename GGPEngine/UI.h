#pragma once
	
#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>

#include <memory>
#include <functional>
#include "Material.h"
#include "Mesh.h"
#include "InputManager.h"

class UIMaterial : public Material {


	UIMaterial(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile) :
		Material(device, deviceContext, vertexShaderFile, pixelShaderFile) {}


	friend class UI;
	friend class D3DSurface;
};

class D3DSurface : public Awesomium::Surface {
private:
	UI * ui;
public:
	D3DSurface(UI * ui, Awesomium::WebView * view, int width, int height);
	virtual void Paint(unsigned char *srcBuffer, int srcRowSpan, const Awesomium::Rect &srcRect, const Awesomium::Rect &destRect);
	virtual void Scroll(int dx, int dy, const Awesomium::Rect &clip_rect) { }
	virtual ~D3DSurface() { }
};

class D3DSurfaceFactory : public Awesomium::SurfaceFactory {
public:
	D3DSurfaceFactory(UI * ui);
	virtual Awesomium::Surface * CreateSurface(Awesomium::WebView * view, int width, int height);
	virtual void DestroySurface(Awesomium::Surface * surface);

private:
	UI * ui;
};

class RenderEngine; //Forward declaration
typedef GPPEngineAPI void(*JSFunctionCallback)();

class UI : public Awesomium::JSMethodHandler
{
private:
	Awesomium::WebCore* webCore;
	std::unique_ptr<Awesomium::WebView, void(*)(Awesomium::WebView *)> view;
	LPCWSTR url;

	std::unique_ptr<D3DSurfaceFactory> surfaceFactory;
	D3DSurface * surface;

	int m_bossHealth;
	bool isLoading, forceReload;

	RenderEngine* e;
	Material * material;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3D11Texture2D* textureResource;
	ID3D11Texture2D* stagingResource;
	
	void SetView();

	void CreateTexture2DResource(int width, int height, const char* name);
	void UpdateResourceFromBuffer(unsigned char *srcBuffer, int srcRowSpan, const Awesomium::Rect &srcRect, const Awesomium::Rect &destRect);

	bool wmMouseMoveHook(WPARAM wParam, LPARAM lParam);
	bool wmMouseButtonDownHook(WPARAM wParam, LPARAM lParam, MouseButton btn);
	bool wmMouseButtonUpHook(WPARAM wParam, LPARAM lParam, MouseButton btn);

	Awesomium::JSValue          m_jsApp;
	std::map<std::string, JSFunctionCallback> jsUserFunctions;

	void OnMethodCall(Awesomium::WebView * caller, unsigned remoteObjectId, Awesomium::WebString const & methodName, Awesomium::JSArray const & args);

	Awesomium::JSValue OnMethodCallWithReturnValue(Awesomium::WebView * caller, unsigned remoteObjectId, Awesomium::WebString const & methodName, Awesomium::JSArray const & args);

public:
	UI(RenderEngine* e);
	~UI();
	bool Initialize();
	void Draw();
	void Update();

	void SetURL(LPCWSTR url);
	bool IsUIPixel(unsigned x, unsigned y);
	bool ExecuteJavascript(std::string javascript);
	bool RegisterJavascriptFunction(std::string functionName, JSFunctionCallback functionPointer);

	friend class RenderEngine;
	friend class D3DSurface;
};