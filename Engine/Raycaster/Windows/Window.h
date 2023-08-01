#pragma once
#include <windows.h>
#include <cstdint>
#include <functional>

LRESULT CALLBACK WindowProc(HWND hWind, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Window
{
public:
	Window(HINSTANCE hInstance, LPCWSTR className, int x, int y, int width, int height, int bufferW, int bufferH);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window();

	bool HandleMessages(std::function<bool(const MSG&)> callbackFunc);
	void Flush();	

	uint32_t* m_pixelBuffer;
	int m_windowW;
	int m_windowH;
	int m_pixelBufferH;
	int m_pixelBufferW;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	LPCWSTR m_className;
};