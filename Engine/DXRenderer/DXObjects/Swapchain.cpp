#include "Swapchain.h"

Swapchain::Swapchain(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC1 desc;

	// clear out the struct for use
	memset(&desc, 0, sizeof(DXGI_SWAP_CHAIN_DESC1));

	// fill the swap chain description struct
	desc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.BufferCount = 2;
	desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.Flags = 0;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Scaling = DXGI_SCALING_NONE;
	desc.Stereo = false;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT res = s_factory->CreateSwapChainForHwnd(s_device, hWnd, &desc, NULL, NULL, m_swapchain.reset());
	ALWAYS_ASSERT(res >= 0 && "CreateSwapChainForHwnd");
}

Swapchain::~Swapchain()
{
}

void Swapchain::Present()
{
	m_swapchain->Present(0, 0);
}
