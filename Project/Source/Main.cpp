#include <iostream>
#include "ApplicationDX.h"
#include "Common/Engine.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE PrevInstance, PWSTR CmdLine, int CmdShow)
{
	FILE* fp;
	AllocConsole();
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	auto& engine = Engine::GetInstance();
	engine.Init();
	
	ApplicationDX application = ApplicationDX(hInstance, L"DirectX11Renderer", 50, 50, 800, 800);
	application.InitSetup();
	
	application.Start();

	engine.DeInit();

	return 0;
}