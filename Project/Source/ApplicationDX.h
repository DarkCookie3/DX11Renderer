#pragma once
#include "DXRenderer/CustomTypes/Model.h"
#include <DXRenderer/Window/WindowDX.h>
#include <Common/Timer.h>
#include <DXRenderer/Managers/ShadersManager.h>
#include <DXRenderer/DXObjects/ConstantBuffer.h>
#include "DXRenderer/CustomTypes/Vertex.h"


class ApplicationDX
{
public:
	ApplicationDX(HINSTANCE hInstance, LPCWSTR className, int x, int y, int width, int height);
	virtual ~ApplicationDX();

	void InitSetup();
	bool ProcessInput(const MSG& msg);

	void Start();
private:
	WindowDX m_window;
};