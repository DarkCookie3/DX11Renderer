#pragma once
#include "../DXRenderer/Utils/d3d.h"
#include <functional>
#include "../DXRenderer/Window/WindowDX.h"
#include "Timer.h"
#include "../DXRenderer/CustomTypes/CameraDX.h"
#include "Keys.h"
#include "../DXRenderer/CustomTypes/TriangleOctreeDX.h"
#include "MouseCursor.h"
#include <assimp/Importer.hpp>

class Engine
{
public:
	static Engine& GetInstance()
	{
		static Engine engine;
		return engine;
	}
	Engine& operator=(const Engine&) = delete;
	Engine(const Engine&) = delete;

	void Init();
	void DeInit();

	void SetInputHandler(std::function<bool(const MSG&)> inputHandlerCallback);
	void SetWindowAndCamera(WindowDX* const& window, const CameraDX& camera, float cameraMovementSpeed, float cameraRotationSpeed);
	void UpdateBufferWithCameraMatrices();

	void ChangeCameraState();
	void CastRayToMove(uint32_t x, uint32_t y, MeshIntersection& rec);
	void DragObject(MeshIntersection& capturedObject, float deltaX, float deltaY);

	void Run();

	CameraDX& GetCamera() { return m_camera; }
	WindowDX& GetWindow() { return *m_window; }
	Timer& GetTimer() { return m_timer; }
private:
	std::function<bool(const MSG&)> m_inputHandlerCallback;
	Timer m_timer;
	WindowDX* m_window;
	CameraDX m_camera;
	float m_cameraMovementSpeed;

	Engine() {}

public:
	PressedKeys m_keys;
	MouseCursor m_cursor;
};