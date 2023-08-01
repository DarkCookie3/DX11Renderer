#pragma once
#include <cstdint>
#include <functional>
#include "../Utils/win.h"
#include "../Utils/d3d.h"
#include "../DXObjects/Swapchain.h"
#include "../DXObjects/RenderTarget.h"
#include "../Utils/imgui/imgui.h"
#include "../Utils/imgui/imgui_impl_win32.h"
#include "../Utils/imgui/imgui_impl_dx11.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProcDX(HWND hWind, UINT uMsg, WPARAM wParam, LPARAM lParam);

class WindowDX
{
public:
	WindowDX(HINSTANCE hInstance, LPCWSTR className, int x, int y, int width, int height);
	WindowDX(const WindowDX&) = delete;
	WindowDX& operator=(const WindowDX&) = delete;
	~WindowDX();

	bool HandleMessages(std::function<bool(const MSG&)> callbackFunc);
	void Present();
	void ResizeBackBuffer(uint32_t width, uint32_t height);
	void ResizeViewport(uint32_t width, uint32_t height);

	int m_windowW;
	int m_windowH;
	bool resized = false;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	LPCWSTR m_className;
	Swapchain m_swapchain;
	RenderTarget m_backbufferRenderTargetHDR;
	RenderTarget m_backbufferRenderTargetLDR;
private:
	void InitSwapchain();
	void InitBackBuffer();
};