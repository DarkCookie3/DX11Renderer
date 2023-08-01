#pragma once
#include "../Utils/d3d.h"
#include "Swapchain.h"

class RenderTarget
{
public:
	RenderTarget() = default;
	RenderTarget(Swapchain& swapchain, DXGI_FORMAT format, uint32_t width, uint32_t height, bool onBackbuffer = false);
	~RenderTarget();

	void Bind();
	void Bind(DxResPtr<ID3D11DepthStencilView> depthStencilView);
	void Resize(Swapchain& swapchain, uint32_t width, uint32_t height);
	void Clear(FLOAT color[4]);

	DxResPtr<ID3D11Texture2D> GetBackBuffer();
	DxResPtr<ID3D11Texture2D> GetDepthStencilBuffer();

	friend class PostProcess;
private:
	DxResPtr<ID3D11RenderTargetView> m_renderTargetView;
	DxResPtr<ID3D11DepthStencilView> m_depthStencilView;

	DxResPtr<ID3D11Texture2D> m_backBufferTexture;
	DxResPtr<ID3D11Texture2D> m_depthStencilTexture;

	bool onBackbuffer;

	void InitBuffers(Swapchain& swapchain, DXGI_FORMAT format, uint32_t width, uint32_t height);
};