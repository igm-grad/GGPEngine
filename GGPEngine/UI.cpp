#include "UI.h"
#include "RenderEngine.h"
#include <Windowsx.h>
#include <memory>

#define URL     "file:///./test.html"


#pragma region D3Surface
D3DSurface::D3DSurface(UI * ui, Awesomium::WebView * view, int width, int height) : ui(ui) {
	ui->CreateTexture2DResource(width, height, "uiTexture");
}

void D3DSurface::Paint(unsigned char *srcBuffer, int srcRowSpan, const Awesomium::Rect &srcRect, const Awesomium::Rect &destRect) {
	ui->UpdateResourceFromBuffer(srcBuffer, srcRowSpan, srcRect, destRect);
}
#pragma endregion

#pragma region D3SurfaceFactory
D3DSurfaceFactory::D3DSurfaceFactory(UI * ui) : ui(ui) {
}

Awesomium::Surface * D3DSurfaceFactory::CreateSurface(Awesomium::WebView * view, int width, int height) {
	return new D3DSurface(ui, view, width, height);
}

void D3DSurfaceFactory::DestroySurface(Awesomium::Surface * surface) {
	delete surface;
}
#pragma endregion

UI::UI(RenderEngine* e) : view(nullptr, [](Awesomium::WebView * ptr) { ptr->Destroy(); })
{
	webCore = Awesomium::WebCore::Initialize(Awesomium::WebConfig());
	m_bossHealth = 100;
	isLoading = false;
	forceReload = true;

	this->url = URL;
	this->e = e;
	this->device = e->device;
	this->deviceContext = e->deviceContext;
}

UI::~UI()
{
	if (webCore)
		webCore->Shutdown();
}

Awesomium::JSValue UI::OnSkill(Awesomium::WebView * view, Awesomium::JSArray const & args) {
	if (args.size() == 0)
		return Awesomium::JSValue();

	Awesomium::JSValue const & arg = args[0];
	if (!arg.IsInteger())
		return Awesomium::JSValue();

	switch (arg.ToInteger())
	{
	case 1:
		--m_bossHealth;
		UpdateBossHealth();
		break;
	default:
		break;
	}

	return Awesomium::JSValue();
}

void UI::UpdateBossHealth() {
	auto javascript = std::string("$('#progressbar').progressbar({ value: ") + std::to_string(m_bossHealth) + "}); ";
	view->ExecuteJavascript(Awesomium::ToWebString(javascript), Awesomium::WSLit(""));
}

bool UI::Initialize() {
	material = new UIMaterial(device, deviceContext, L"UIVertexShader.cso", L"UIPixelShader.cso");
	material->SetSampler("uiSampler");

	surfaceFactory = std::make_unique<D3DSurfaceFactory>(this);
	webCore->set_surface_factory(surfaceFactory.get());

	if (url) {
		SetView();
	} else
		return false;

	return true;
}

void UI::SetView() {
	view.reset(webCore->CreateWebView(e->windowWidth, e->windowHeight, nullptr, Awesomium::kWebViewType_Offscreen));

	view->SetTransparent(true);
	view->LoadURL(Awesomium::WebURL(Awesomium::WSLit(URL)));
}

void UI::SetURL(const char * url) {
	this->url = url;
}

void UI::Draw() {
	material->sVertexShader->SetShader();
	material->UpdatePixelShaderResources();
	material->UpdatePixelShaderSamplers();
	material->sPixelShader->SetShader();

	deviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	deviceContext->IASetIndexBuffer(nullptr, (DXGI_FORMAT)0, 0);
	deviceContext->IASetInputLayout(nullptr);

	deviceContext->Draw(3, 0);
}

void UI::Update() {
	if (forceReload) {
		forceReload = false;
		SetView();
	}

	webCore->Update();
		
	if (view->IsLoading()) {
		isLoading = true;
		webCore->Update();
	}
	else if (isLoading) {
		isLoading = false;

		//JS
		view->set_js_method_handler(this);
		m_jsApp = view->CreateGlobalJavascriptObject(Awesomium::WSLit("app"));

		Awesomium::JSObject & appObject = m_jsApp.ToObject();
		appObject.SetCustomMethod(Awesomium::WSLit("skill"), false);

		JsCallerKey key(appObject.remote_id(), Awesomium::WSLit("skill"));
		m_jsFunctions[key] = std::bind(&UI::OnSkill, this, std::placeholders::_1, std::placeholders::_2);

		webCore->Update();
		surface = static_cast<D3DSurface *>(view->surface());
	}	
}

void UI::CreateTexture2DResource(int width, int height, const char* name)
{
	if (textureResource) {
		textureResource->Release();
	}
	if (stagingResource) {
		stagingResource->Release();
	}

	device->CreateTexture2D(&CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_B8G8R8A8_UNORM, width, height, 1, 1), nullptr, &textureResource);
	material->SetResource(textureResource, "uiTexture");
	device->CreateTexture2D(&CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_B8G8R8A8_UNORM, width, height, 1, 1, 0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE), nullptr, &stagingResource);
}

void UI::UpdateResourceFromBuffer(unsigned char *srcBuffer, int srcRowSpan, const Awesomium::Rect &srcRect, const Awesomium::Rect &destRect)
{
	if (!textureResource) {
		return;
	}

	auto box = CD3D11_BOX(destRect.x, destRect.y, 0, destRect.x + destRect.width, destRect.y + destRect.height, 1);

	D3D11_MAPPED_SUBRESOURCE resource;
	deviceContext->Map(stagingResource, 0, D3D11_MAP_WRITE, 0, &resource);

	auto srcStartingOffset = srcRowSpan * srcRect.y + srcRect.x * 4;
	uint8_t * srcPtr = srcBuffer + srcStartingOffset;

	auto dstStartingOffset = resource.RowPitch * destRect.y + destRect.x * 4;
	uint8_t * dataPtr = reinterpret_cast<uint8_t *>(resource.pData) + dstStartingOffset;

	for (int i = 0; i < destRect.height; ++i) {
		memcpy(dataPtr + resource.RowPitch * i, srcPtr + srcRowSpan * i, destRect.width * 4);
	}
	deviceContext->Unmap(stagingResource, 0);

	deviceContext->CopySubresourceRegion(textureResource, 0, destRect.x, destRect.y, 0, stagingResource, 0, &box);
}

bool UI::IsUIPixel(unsigned x, unsigned y) {
	if (!stagingResource) return false;

	D3D11_MAPPED_SUBRESOURCE resource;
	deviceContext->Map(stagingResource, 0, D3D11_MAP_READ, 0, &resource);

	auto startingOffset = (e->windowWidth * y + x) * 4;
	uint8_t * dataPtr = reinterpret_cast<uint8_t *>(resource.pData) + startingOffset;
	bool result = *dataPtr != 0;

	deviceContext->Unmap(stagingResource, 0);

	return result;
}


#pragma region Mouse Processing
bool UI::wmMouseMoveHook(WPARAM wParam, LPARAM lParam) {
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);

	if (view) {
		if (wParam && MK_LBUTTON)
			view->InjectMouseDown(Awesomium::kMouseButton_Left);

		view->InjectMouseMove(xPos, yPos);
		return false;
	}
}

bool UI::wmMouseButtonDownHook(WPARAM wParam, LPARAM lParam, MouseButton btn) {
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam); 
	
	bool isUI = IsUIPixel(xPos, yPos);

	if (view) {
		if (isUI) {
			switch (btn) {
			case MOUSEBUTTON_RIGHT:
				view->InjectMouseDown(Awesomium::kMouseButton_Right);
				break;
			case MOUSEBUTTON_MIDDLE:
				view->InjectMouseDown(Awesomium::kMouseButton_Middle);
				break;
			default:
			case MOUSEBUTTON_LEFT:
				view->InjectMouseDown(Awesomium::kMouseButton_Left);
				break;
			}
		}
	}
	return isUI;
}

bool UI::wmMouseButtonUpHook(WPARAM wParam, LPARAM lParam, MouseButton btn) {
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);

	bool isUI = IsUIPixel(xPos, yPos);

	if (view) {
		if (isUI) {
			switch (btn) {
			case MOUSEBUTTON_RIGHT:
				view->InjectMouseUp(Awesomium::kMouseButton_Right);
				break;
			case MOUSEBUTTON_MIDDLE:
				view->InjectMouseUp(Awesomium::kMouseButton_Middle);
				break;
			default:
			case MOUSEBUTTON_LEFT:
				view->InjectMouseUp(Awesomium::kMouseButton_Left);
				break;
			}
		}
	}
	return isUI;
}
#pragma endregion