#pragma once
#include "../Utils/d3d.h"

class Swapchain
{
public:
	Swapchain() = default;
	Swapchain(HWND hWnd);
	~Swapchain();

	void Present();

	IDXGISwapChain1* operator->()
	{
		return m_swapchain.ptr();
	}
private:
	DxResPtr<IDXGISwapChain1> m_swapchain;
};