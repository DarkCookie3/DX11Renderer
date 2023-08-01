#include "RenderTarget.h"

RenderTarget::RenderTarget(Swapchain& swapchain, DXGI_FORMAT format, uint32_t width, uint32_t height, bool onBackbuffer) : onBackbuffer(onBackbuffer)
{
	InitBuffers(swapchain, format, width, height);
}

RenderTarget::~RenderTarget()
{
}

void RenderTarget::Bind()
{
	const auto backbufferRenderTargetView = m_renderTargetView.ptr();
	s_devcon->OMSetRenderTargets(1, &backbufferRenderTargetView, m_depthStencilView);
}

void RenderTarget::Bind(DxResPtr<ID3D11DepthStencilView> depthStencilView)
{
	const auto backbufferRenderTargetView = m_renderTargetView.ptr();
	s_devcon->OMSetRenderTargets(1, &backbufferRenderTargetView, depthStencilView);
}

void RenderTarget::Resize(Swapchain& swapchain, uint32_t width, uint32_t height)
{
	D3D11_TEXTURE2D_DESC desc;
	m_backBufferTexture->GetDesc(&desc);
	InitBuffers(swapchain, desc.Format, width, height);
}

void RenderTarget::Clear(FLOAT color[4])
{
	s_devcon->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
	s_devcon->ClearRenderTargetView(m_renderTargetView, color);
}

DxResPtr<ID3D11Texture2D> RenderTarget::GetBackBuffer()
{
	return m_backBufferTexture;
}

DxResPtr<ID3D11Texture2D> RenderTarget::GetDepthStencilBuffer()
{
	return m_depthStencilTexture;
}

void RenderTarget::InitBuffers(Swapchain& swapchain, DXGI_FORMAT format, uint32_t width, uint32_t height)
{
	auto backbufferTextureReleased = m_backBufferTexture.reset();
	auto depthStencilTextureReleased = m_depthStencilTexture.reset();
	auto depthStencilReleased = m_depthStencilView.reset();
	auto backbufferReleased = m_renderTargetView.reset();

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	if (onBackbuffer)
	{
		swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		HRESULT result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backbufferTextureReleased);
		ALWAYS_ASSERT(result >= 0);
	}
	else
	{
		DxResPtr<ID3D11Texture2D> backBuffer;
		HRESULT result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.reset());
		ALWAYS_ASSERT(result >= 0);
		D3D11_TEXTURE2D_DESC desc;
		backBuffer->GetDesc(&desc);
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Format = format;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		s_device->CreateTexture2D(&desc, NULL, backbufferTextureReleased);
	}

	s_device->CreateRenderTargetView(m_backBufferTexture, NULL, backbufferReleased);

	s_device->CreateTexture2D(&descDepth, NULL, depthStencilTextureReleased);

	s_device->CreateDepthStencilView(m_depthStencilTexture, &descDSV, depthStencilReleased);
}
