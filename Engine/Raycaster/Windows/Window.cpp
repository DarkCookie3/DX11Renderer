#include "Window.h"
#include "../Utils/Camera.h"
#include <iostream>


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static Window* pWindow;
	static bool fullscreen = false;

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
		pWindow = reinterpret_cast<Window*>(ptr);

		pWindow->m_pixelBufferW = float(pWindow->m_pixelBufferW) * newW / pWindow->m_windowW;
		pWindow->m_pixelBufferH = float(pWindow->m_pixelBufferH) * newH / pWindow->m_windowH;
		pWindow->m_windowW = newW;
		pWindow->m_windowH = newH;

		delete pWindow->m_pixelBuffer;
		pWindow->m_pixelBuffer = new uint32_t[pWindow->m_pixelBufferW * pWindow->m_pixelBufferH];
		break;
	}
	case WM_CREATE:
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
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

Window::Window(HINSTANCE hInstance, LPCWSTR className,  int x, int y, int width, int height, int bufferW, int bufferH) 
	: m_hInstance(hInstance), m_className(className), m_pixelBufferW(bufferW), m_pixelBufferH(bufferH), m_windowW(width), m_windowH(height)
{
	WNDCLASS wndClass {};
	wndClass.lpszClassName = className;
	wndClass.hInstance = m_hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wndClass.lpfnWndProc = WindowProc;

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
		L"Raytracer",
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

	m_pixelBuffer = new uint32_t[m_pixelBufferW * m_pixelBufferH];
	ShowWindow(m_hWnd, SW_SHOW);
}

Window::~Window()
{
	delete[] m_pixelBuffer;
	UnregisterClass(m_className, m_hInstance);
}

void Window::Flush()
{
	RECT clientRect;
	GetClientRect(m_hWnd, &clientRect);
	HDC DeviceContext = GetDC(m_hWnd);
	BITMAPINFO BitmapInfo;
	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = m_pixelBufferW;
	BitmapInfo.bmiHeader.biHeight = -m_pixelBufferH;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
	StretchDIBits(DeviceContext,
		            0, 0,
					clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,  
		            0, 0,
					m_pixelBufferW, m_pixelBufferH,
		            m_pixelBuffer, &BitmapInfo,
		            DIB_RGB_COLORS, SRCCOPY);
}

bool Window::HandleMessages(std::function<bool(const MSG&)> callbackFunc)
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
	callbackFunc(MSG{});

	return true;
}
