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

class UI : public Awesomium::JSMethodHandler
{
private:
	Awesomium::WebCore* webCore;
	std::unique_ptr<Awesomium::WebView, void(*)(Awesomium::WebView *)> view;
	const char * url;

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


	typedef std::pair<unsigned, Awesomium::WebString> JsCallerKey;
	typedef std::function<Awesomium::JSValue(Awesomium::WebView *, Awesomium::JSArray const &)> JsFunction;
	
	std::map<JsCallerKey, JsFunction> m_jsFunctions;
	std::map<JsCallerKey, JsFunction> m_jsFunctionsWithRetValue;
	Awesomium::JSValue          m_jsApp;

	void OnMethodCall(Awesomium::WebView * caller, unsigned remoteObjectId, Awesomium::WebString const & methodName, Awesomium::JSArray const & args) {
		JsCallerKey key(remoteObjectId, methodName);
		auto itor = m_jsFunctions.find(key);

		if (itor != m_jsFunctions.end()) {
			itor->second(caller, args);
		}
	}

	Awesomium::JSValue Awesomium::JSMethodHandler::OnMethodCallWithReturnValue(Awesomium::WebView * caller, unsigned remoteObjectId, Awesomium::WebString const & methodName, Awesomium::JSArray const & args) {
		JsCallerKey key(remoteObjectId, methodName);
		auto itor = m_jsFunctionsWithRetValue.find(key);

		if (itor != m_jsFunctionsWithRetValue.end()) {
			return itor->second(caller, args);
		}

		return Awesomium::JSValue();
	}

	Awesomium::JSValue OnSkill(Awesomium::WebView * view, Awesomium::JSArray const & args);
	void UpdateBossHealth();

public:
	UI(RenderEngine* e);
	~UI();
	bool Initialize();
	void Draw();
	void Update();

	void SetURL(const char * url);
	bool IsUIPixel(unsigned x, unsigned y);

	friend class RenderEngine;
	friend class D3DSurface;
};