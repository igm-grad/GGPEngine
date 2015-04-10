#include "UI.h"
#include <memory>

#define URL     "file:///./test.html"

class D3DSurface : public Awesomium::Surface {
public:
	D3DSurface(UI * ui, Awesomium::WebView * view, int width, int height) : ui(ui) {
		ui->material->CreateTexture2D(width, height, "uiTexture");
	}

	virtual void Paint(unsigned char *srcBuffer, int srcRowSpan, const Awesomium::Rect &srcRect, const Awesomium::Rect &destRect) {
		auto box = CD3D11_BOX(destRect.x, destRect.y, 0, destRect.x + destRect.width, destRect.y + destRect.height, 1);

		// 4 bytes per pixel, srcRowSpan is already in bytes.
		auto startingOffset = srcRowSpan * srcRect.y + srcRect.x * 4;

		ui->material->UpdateResourceFromBuffer(srcBuffer + startingOffset, &box, srcRowSpan, "uiTexture");
	}

	virtual void Scroll(int dx, int dy, const Awesomium::Rect &clip_rect) {

	}

	virtual ~D3DSurface() { }

private:
	UI *							  ui;
};

class D3DSurfaceFactory : public Awesomium::SurfaceFactory {
public:
	D3DSurfaceFactory(UI * ui) : ui(ui) {

	}

	virtual Awesomium::Surface * CreateSurface(Awesomium::WebView * view, int width, int height) {
		return new D3DSurface(ui, view, width, height);
	}

	virtual void DestroySurface(Awesomium::Surface * surface) {
		delete surface;
	}

private:
	UI * ui;
};

UI::UI(RenderEngine* e)
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

bool UI::Initialize() {
	material = new Material(device, deviceContext, L"UIVertexShader.cso", L"UIPixelShader.cso");
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
	view = webCore->CreateWebView(e->windowWidth, e->windowHeight, nullptr, Awesomium::kWebViewType_Offscreen);
	view->SetTransparent(true);
	view->LoadURL(Awesomium::WebURL(Awesomium::WSLit(URL)));
	surface = static_cast<D3DSurface *>(view->surface());
}

void UI::SetURL(const char * url) {
	this->url = url;
}

UI::~UI()
{
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
		if (view) view->Destroy();
		SetView();
	}

	webCore->Update();
		
	//void UpdateBossHealth() {
	auto javascript = std::string("$('#progressbar').progressbar({ value: ") + std::to_string(m_bossHealth) + "}); ";
	view->ExecuteJavascript(Awesomium::ToWebString(javascript), Awesomium::WSLit(""));
	m_bossHealth--;
	if (m_bossHealth == 0) m_bossHealth = 100;
	//}

	if (view->IsLoading()) {
		isLoading = true;
		webCore->Update();
	}
	else if (isLoading) {
		isLoading = false;
		webCore->Update();
		surface = static_cast<D3DSurface *>(view->surface());
	}	
}


