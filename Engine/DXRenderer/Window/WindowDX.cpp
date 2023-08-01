#include "WindowDX.h"
#include <iostream>

LRESULT CALLBACK WindowProcDX(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static WindowDX* pWindow;
	static bool fullscreen = false;

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
	{
		return true;
	}

	switch (uMsg)
	{
	case WM_SIZE:
	{
		if (wParam == SIZE_MAXIMIZED || fullscreen)
		{
			fullscreen = !fullscreen;
			SendMessage(hWnd, WM_EXITSIZEMOVE, wParam, lParam);
		}
		break;
	}
	case WM_EXITSIZEMOVE:
	{
		RECT rect{};
		GetClientRect(hWnd, &rect);

		int newW = rect.right - rect.left;
		int newH = rect.bottom - rect.top;
		if (newW <= 1 || newH <= 1)
		{
			break;
		}

		LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
		pWindow = reinterpret_cast<WindowDX*>(ptr);
		pWindow->ResizeBackBuffer(newW, newH);
		break;
	}
	case WM_CREATE:
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pWindow = reinterpret_cast<WindowDX*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pWindow);
		break;
	}
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

WindowDX::WindowDX(HINSTANCE hInstance, LPCWSTR className, int x, int y, int width, int height)
	: m_hInstance(hInstance), m_className(className), m_windowW(width), m_windowH(height)
{
	WNDCLASS wndClass {};
	wndClass.lpszClassName = className;
	wndClass.hInstance = m_hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wndClass.lpfnWndProc = WindowProcDX;

	RegisterClassW(&wndClass);

	DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	AdjustWindowRect(&rect, style, false);

	m_hWnd = CreateWindowEx(
		0,
		className,
		L"DirectXRenderer",
		style,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		m_hInstance,
		this
		);

	ShowWindow(m_hWnd, SW_SHOW);
	InitSwapchain();
	InitBackBuffer();

	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	s_devcon->RSSetViewports(1, &vp);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(s_device, s_devcon);
}

WindowDX::~WindowDX()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	UnregisterClass(m_className, m_hInstance);
}

void WindowDX::Present()
{
	m_swapchain.Present();
	resized = false;
}

void WindowDX::InitSwapchain()
{
	m_swapchain = Swapchain(m_hWnd);
}

void WindowDX::InitBackBuffer()
{
	m_backbufferRenderTargetLDR = RenderTarget(m_swapchain, DXGI_FORMAT_R8G8B8A8_UNORM, m_windowW, m_windowH, true);
	m_backbufferRenderTargetHDR = RenderTarget(m_swapchain, DXGI_FORMAT_R16G16B16A16_FLOAT, m_windowW, m_windowH);
	m_backbufferRenderTargetHDR.Bind();
}

void WindowDX::ResizeBackBuffer(uint32_t width, uint32_t height)
{
	m_backbufferRenderTargetLDR.Resize(m_swapchain, width, height);
	m_backbufferRenderTargetHDR.Resize(m_swapchain, width, height);
	m_backbufferRenderTargetHDR.Bind();

	ResizeViewport(width, height);

	m_windowW = width;
	m_windowH = height;
	resized = true;
}

void WindowDX::ResizeViewport(uint32_t width, uint32_t height)
{
	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	s_devcon->RSSetViewports(1, &vp);
}

bool WindowDX::HandleMessages(std::function<bool(const MSG&)> callbackFunc)
{
	MSG msg{};

	while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
	{
		if (!callbackFunc(msg))
		{
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}
