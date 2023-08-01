#include "Application.h"
#include <iostream>
#include <Raycaster/Source/Entities/SphereEntity.h>
#include <Raycaster/Source/Entities/TriangleEntity.h>

Application::Application(uint32_t threads_num, HINSTANCE hInstance, LPCWSTR className, int x, int y, int width, int height, int bufferW, int bufferH)
	: m_window(hInstance, className, x, y, width, height, bufferW, bufferH)
{
}

Application::~Application()
{
}

bool Application::ProcessInput(const MSG& msg)
{
	static IObjectMover* capturedObjectMover;
	static HitRecord capturedPoint;
	static bool RMBpressed = false;
	static bool LMBpressed = false;
	static int currentMouseX;
	static int currentMouseY;
	static int lastMouseX;
	static int lastMouseY;
	int forward = 0;
	int right = 0;
	int up = 0;
	int roll = 0;

	if (msg.message == WM_QUIT)
	{
		return false;
	}
	switch (msg.message)
	{
	case WM_MOUSEMOVE:
	{
		if (LMBpressed)
		{
			lastMouseX = currentMouseX;
			lastMouseY = currentMouseY;
			currentMouseX = LOWORD(msg.lParam);
			currentMouseY = HIWORD(msg.lParam);
		}
		break;
	}
	case WM_RBUTTONDOWN:
		RMBpressed = true;
		break;
	case WM_LBUTTONDOWN:
		LMBpressed = true;
		currentMouseX = LOWORD(msg.lParam);
		currentMouseY = HIWORD(msg.lParam);
		lastMouseX = currentMouseX;
		lastMouseY = currentMouseY;
		capturedObjectMover = m_scene.CastRayToMove(m_camera, capturedPoint, m_window.m_windowW, m_window.m_windowH, currentMouseX, currentMouseY);
		break;
	case WM_RBUTTONUP:
		RMBpressed = false;
		break;
	case WM_LBUTTONUP:
		capturedObjectMover = nullptr;
		LMBpressed = false;
		break;
	case WM_KEYDOWN:
		switch (msg.wParam)
		{
		case 0x57: //W
			forward = 1;
			break;
		case 0x41: //A
			right = -1;
			break;
		case 0x53: //S
			forward = -1;
			break;
		case 0x44: //D
			right = 1;
			break;
		case VK_SPACE:
			up = 1;
			break;
		case VK_CONTROL:
			up = -1;
			break;
		case 0x45: //E
			roll = -1;
			break;
		case 0x51: //Q
			roll = 1;
			break;
		}
	}
	if (RMBpressed)
	{
		m_camera.Move(float(forward) * m_camera.m_transform.Forward() * m_cameraMovementSpeed * m_timer.GetDeltaTime());
		m_camera.Move(float(right) * m_camera.m_transform.Right() * m_cameraMovementSpeed * m_timer.GetDeltaTime());
		m_camera.Move(float(up) * m_camera.m_transform.Up() * m_cameraMovementSpeed * m_timer.GetDeltaTime());
		float rollAngle = float(roll) * m_cameraRotationSpeed * m_timer.GetDeltaTime();
		float pitchAngle = 0.0f;
		float yawAngle = 0.0f;
		if (LMBpressed)
		{
			pitchAngle = (currentMouseY - m_window.m_windowH / 2) / 10.0f * m_timer.GetDeltaTime();
			yawAngle = (currentMouseX - m_window.m_windowW / 2) / 10.0f * m_timer.GetDeltaTime();
		}
		m_camera.Rotate(rollAngle, glm::vec3(0, 0, 1));
		m_camera.Rotate(pitchAngle, glm::vec3(1, 0, 0));
		m_camera.Rotate(yawAngle, glm::vec3(0, 1, 0));
	}
	else if (LMBpressed)
	{
		float deltaX = currentMouseX - lastMouseX;
		float deltaY = lastMouseY - currentMouseY;
		glm::vec3 HitVec = capturedPoint.m_point - m_camera.m_transform.position;
		float aspectRatio = float(m_window.m_windowW) / m_window.m_windowH;
		float worldDeltaX = 2 * glm::tan(glm::radians(m_camera.m_fov/2)) * m_camera.m_nearPlaneDist * aspectRatio * deltaX / m_window.m_windowW;
		float worldDeltaY = 2 * glm::tan(glm::radians(m_camera.m_fov/2)) * m_camera.m_nearPlaneDist * deltaY / m_window.m_windowH;
		HitVec = m_camera.generateViewMat() * glm::vec4(HitVec, 0);
		worldDeltaY *= HitVec.z / m_camera.m_nearPlaneDist;
		worldDeltaX *= HitVec.z / m_camera.m_nearPlaneDist;
		if (capturedObjectMover != nullptr)
		{
			capturedObjectMover->Move(m_camera.m_transform.Up() * worldDeltaY + m_camera.m_transform.Right() * worldDeltaX);
			capturedPoint.m_point += m_camera.m_transform.Up() * worldDeltaY + m_camera.m_transform.Right() * worldDeltaX;
		}
	}
	lastMouseX = currentMouseX;
	lastMouseY = currentMouseY;
	return true;
}

void Application::InitSetup()
{
	m_camera = Camera(glm::vec3(0, 2, -10), glm::vec3(0, 2, 1), 0.1f, 100.0f, 60.0f, 8.0f / 6.0f);

	std::shared_ptr<Material> RedMat{ new Material{ 0xffff0000, 0xffffffff, 64 } };
	std::shared_ptr<Material> WhiteMat{ new Material{ 0xffffffff, 0xffffffff, 64 } };

	std::shared_ptr<LightMat> whiteLight{ new LightMat{ 0.8f, 0xffffffff } };
	std::shared_ptr<LightMat> blueLight{ new LightMat{ 0.8f, 0xff7DF9FF } };
	std::shared_ptr<LightMat> pinkLight{ new LightMat{ 0.8f, 0xffD8004A } };

	m_scene.m_spheres.push_back(SphereEntity{glm::vec3(-10.0f, 5.0f, 10.0f), 3.0f, RedMat});
	m_scene.m_spheres.push_back(SphereEntity{ glm::vec3(10.0f, 5.0f, -10.0f), 3.0f, WhiteMat });
	m_scene.m_planes.push_back(PlaneEntity{ glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), WhiteMat });
	m_scene.m_pointLights.push_back(PointLightEntity{ glm::vec3(10.0f, 5.0f, -5.0f), 0.5f, blueLight });
	m_scene.m_spotLights.push_back(SpotLightEntity{ glm::vec3(0.0f, 15.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::radians(20.0f), 0.5f, whiteLight });
	m_scene.m_pointLights.push_back(PointLightEntity{ glm::vec3(-10.0f, 5.0f, -5.0f), 0.5f, pinkLight});
	//m_scene.m_directLights.push_back(DirLightEntity{ glm::vec3(1.0f, -1.0f, -1.0f), whiteLight });

	auto cubePtr = std::make_shared<std::vector<Triangle>>(std::vector<Triangle>{
		Triangle{ glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(1, -1, 1) },
		Triangle{ glm::vec3(-1, -1, -1), glm::vec3(-1, -1, 1), glm::vec3(1, -1, 1) },
		Triangle{ glm::vec3(-1, -1, 1), glm::vec3(1, -1, 1), glm::vec3(1, 1, 1) },
		Triangle{ glm::vec3(-1, -1, 1), glm::vec3(-1, 1, 1), glm::vec3(1, 1, 1) },
		Triangle{ glm::vec3(-1, -1, 1), glm::vec3(-1, -1, -1), glm::vec3(-1, 1, -1) },
		Triangle{ glm::vec3(-1, -1, 1), glm::vec3(-1, 1, 1), glm::vec3(-1, 1, -1) },
		Triangle{ glm::vec3(1, -1, 1), glm::vec3(1, -1, -1), glm::vec3(1, 1, -1) },
		Triangle{ glm::vec3(1, -1, 1), glm::vec3(1, 1, 1), glm::vec3(1, 1, -1) },
		Triangle{ glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(1, 1, -1) },
		Triangle{ glm::vec3(-1, -1, -1), glm::vec3(-1, 1, -1), glm::vec3(1, 1, -1) },
		Triangle{ glm::vec3(-1, 1, -1), glm::vec3(1, 1, -1), glm::vec3(1, 1, 1) },
		Triangle{ glm::vec3(-1, 1, -1), glm::vec3(-1, 1, 1), glm::vec3(1, 1, 1) }
	});
	auto cubeBox = Box{ glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1) };
	auto cubeTransform1 = Transform{ glm::vec3(0.0f, 3.0f, 5.0f), glm::quat({0.0f, 0.0f, 0.0f }), glm::vec3(4.0f, 4.0f, 4.0f)};
	auto cubeTransform2 = Transform{ glm::vec3(5.0f, 3.0f, 5.0f), glm::quat({0.2f, 0.0f, 0.0f }), glm::vec3(1.2f, 1.0f, 1.2f) };
	auto cubeMesh1 = Mesh{cubePtr, cubeTransform1, cubeBox};
	auto cubeMesh2 = Mesh{cubePtr, cubeTransform2, cubeBox};
	m_scene.m_meshes.push_back({});
	m_scene.m_meshes.push_back({});
	m_scene.m_meshes[0].Initialize(cubeMesh1);
	m_scene.m_meshes[1].Initialize(cubeMesh2);
	m_scene.m_meshes[0].material = WhiteMat;
	m_scene.m_meshes[1].material = WhiteMat;
}

void Application::Run(int targetFPS)
{
	while (true)
	{
		m_timer.SetForCurrentFrame();

		if (!m_window.HandleMessages(std::bind(&Application::ProcessInput, this, std::placeholders::_1)))
		{
			break;
		}

		m_scene.Render(m_window.m_pixelBufferW, m_window.m_pixelBufferH, m_window.m_pixelBuffer, m_camera);
		m_window.Flush();

		m_timer.WaitUntilNextFrame(60);
	}
}
