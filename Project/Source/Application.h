#pragma once

#include <Raycaster/Windows/Window.h>
#include <Raycaster/Render/Scene.h>
#include <Common/Timer.h>
#include <Common/ParallelExecutor.h>

class Application
{
public:
	Application(uint32_t threads_num, HINSTANCE hInstance, LPCWSTR className, int x, int y, int width, int height, int bufferW, int bufferH);
	virtual ~Application();

	void InitSetup();
	bool ProcessInput(const MSG& msg);

	void Run(int targetFPS);
private:
	Camera m_camera;
	float m_cameraMovementSpeed = 10.0f;
	float m_cameraRotationSpeed = 100.0f;

	Scene m_scene;
	Window m_window;
	Timer m_timer;
};