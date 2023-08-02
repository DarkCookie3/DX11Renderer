#include "Engine.h"
#include "../DXRenderer/Managers/ShadersManager.h"
#include "../DXRenderer/DXObjects/ConstantBuffer.h"
#include "../DXRenderer/CustomTypes/Vertex.h"
#include "../DXRenderer/Managers/MeshSystem.h"
#include "../DXRenderer/Managers/Renderer.h"
#include "../DXRenderer/Managers/LightSystem.h"
#include "../DXRenderer/Managers/ReflectionCapturer.h"
#include "../DXRenderer/Managers/ParticleSystem.h"

using namespace DirectX;

void Engine::SetInputHandler(std::function<bool(const MSG&)> inputHandlerCallback)
{
	m_inputHandlerCallback = inputHandlerCallback;
}

void Engine::SetWindowAndCamera(WindowDX* const& window, const CameraDX& camera, float cameraMovementSpeed, float cameraRotationSpeed)
{
	m_window = window;

	m_cameraMovementSpeed = cameraMovementSpeed;
	m_camera = camera;

	UpdateBufferWithCameraMatrices();
}

void Engine::UpdateBufferWithCameraMatrices()
{
	auto& renderer = Renderer::GetInstance();
	auto ConstDataPerView = renderer.GetConstBufferData<CONSTANT_BUFFER_PER_VIEW_STRUCT>(BufferSlots::PER_VIEW);
	auto view = m_camera.generateViewMat();
	auto proj = m_camera.generateProjMatReversedZ(float(m_window->m_windowW) / m_window->m_windowH);
	ConstDataPerView->viewProj = DirectX::XMMatrixMultiply(view, proj);
	ConstDataPerView->viewProjInv = DirectX::XMMatrixInverse(0, ConstDataPerView->viewProj);
	ConstDataPerView->viewInv = m_camera.m_transform.ToMat4();
	ConstDataPerView->projInv = XMMatrixInverse(0, proj);
	ConstDataPerView->view = view;
	ConstDataPerView->proj = proj;
	ConstDataPerView->camera_pos = m_camera.m_transform.position;
	renderer.UpdateConstBufferAtSlot<CONSTANT_BUFFER_PER_VIEW_STRUCT>(BufferSlots::PER_VIEW);
}

void Engine::ChangeCameraState()
{
	if (m_keys.RMB == true)
	{
		static bool flashlightFixedToCamera = false;
		static bool FlockedFlag = false;
		if (m_keys.F)
		{
			if (!FlockedFlag)
			{
				flashlightFixedToCamera = !flashlightFixedToCamera;
				FlockedFlag = true;
			}
		}
		else
		{
			FlockedFlag = false;
		}
		//ADD KEYPRESSED CONDITIONS FOR BETTER PERFOMANCE
		m_camera.Move(m_camera.m_transform.Forward() * float(m_keys.W - m_keys.S) * m_cameraMovementSpeed * m_timer.GetDeltaTime());
		m_camera.Move(m_camera.m_transform.Right() * float(m_keys.D - m_keys.A) * m_cameraMovementSpeed * m_timer.GetDeltaTime());
		m_camera.Move(m_camera.m_transform.Up() * float(m_keys.SPACE - m_keys.CTRL) * m_cameraMovementSpeed * m_timer.GetDeltaTime());
		float pitchAngle = 0.0f;
		float yawAngle = 0.0f;
		if (m_keys.LMB == true)
		{
			static float prevPosX = 0;
			static float prevPosY = 0;
			float verticalCos = XMVector3Dot(XMVectorSet(0, 1, 0, 0), m_camera.m_transform.Forward()).m128_f32[0];
			if (prevPosX != m_cursor.currentMouseX || prevPosY != m_cursor.currentMouseY)
			{
				float aspect = float(m_window->m_windowW) / m_window->m_windowH;
				float distanceToViewportInUnitSphere = cosf(XMConvertToRadians(m_camera.m_fov / 2));
				float viewportHeightInUnitSphere = 2 * sinf(XMConvertToRadians(m_camera.m_fov / 2));
				float viewportWidthInUnitSphere = viewportHeightInUnitSphere * aspect;

				float xA = 2 * float(m_cursor.currentMouseX) / (m_window->m_windowW) - 1;
				xA *= 0.5 * viewportWidthInUnitSphere;
				float yA = -2 * float(m_cursor.currentMouseY) / (m_window->m_windowH) + 1;
				yA *= 0.5 * viewportHeightInUnitSphere;
				XMVECTOR A = XMVectorSet(xA, yA, distanceToViewportInUnitSphere, 1);

				float xB = 2 * float(m_cursor.currentMouseX - m_cursor.deltaX) / (m_window->m_windowW) - 1;
				xB *= 0.5 * viewportWidthInUnitSphere;
				float yB = -2 * float(m_cursor.currentMouseY - m_cursor.deltaY) / (m_window->m_windowH) + 1;
				yB *= 0.5 * viewportHeightInUnitSphere;
				XMVECTOR B = XMVectorSet(xB, yB, distanceToViewportInUnitSphere, 1);

				float newAngleX = acos(XMVector3Dot(XMVectorSet(0, 0, 1, 0), XMVector3Normalize(XMVectorSet(A.m128_f32[0], 0, A.m128_f32[2], 1))).m128_f32[0]);
				float prevAngleX = acos(XMVector3Dot(XMVectorSet(0, 0, 1, 0), XMVector3Normalize(XMVectorSet(B.m128_f32[0], 0, B.m128_f32[2], 1))).m128_f32[0]);

				newAngleX *= A.m128_f32[0] < 0 ? 1 : -1;
				prevAngleX *= B.m128_f32[0] < 0 ? 1 : -1;

				float newAngleY = acos(XMVector3Dot(XMVectorSet(0, 0, 1, 0), XMVector3Normalize(XMVectorSet(0, A.m128_f32[1], A.m128_f32[2], 1))).m128_f32[0]);
				float prevAngleY = acos(XMVector3Dot(XMVectorSet(0, 0, 1, 0), XMVector3Normalize(XMVectorSet(0, B.m128_f32[1], B.m128_f32[2], 1))).m128_f32[0]);

				newAngleY *= A.m128_f32[1] > 0 ? 1 : -1;
				prevAngleY *= B.m128_f32[1] > 0 ? 1 : -1;

				pitchAngle = XMConvertToDegrees(newAngleY - prevAngleY);
				yawAngle = XMConvertToDegrees(newAngleX - prevAngleX) * (1.0f / sqrt(1 - verticalCos * verticalCos));

				prevPosX = m_cursor.currentMouseX;
				prevPosY = m_cursor.currentMouseY;
			}

			if ((verticalCos >= 0.9 && verticalCos <= -0.9) || (verticalCos <= 0.9 && pitchAngle < 0) || (verticalCos >= -0.9 && pitchAngle > 0))
			{
				m_camera.Rotate(pitchAngle, { 1, 0, 0, 0 });
			}
			m_camera.RotateByWorld(yawAngle, { 0, 1, 0, 0 });
		}
		UpdateBufferWithCameraMatrices();
		if (flashlightFixedToCamera)
		{
			auto& lightSystem = LightSystem::GetInstance();
			if (lightSystem.GetZeroIndexedSpotLight())
			{
				lightSystem.GetZeroIndexedSpotLight()->model->GetTransform().position = m_camera.m_transform.position;
				lightSystem.GetZeroIndexedSpotLight()->lightTransform.GetTransform().rotation = m_camera.m_transform.rotation;
			}
		}
	}
	else if (m_window->resized)
	{
		UpdateBufferWithCameraMatrices();
	}
	m_cursor.deltaX = 0;
	m_cursor.deltaY = 0;
}

void Engine::CastRayToMove(uint32_t x, uint32_t y, MeshIntersection& rec)
{
	float aspect = float(m_window->m_windowW) / m_window->m_windowH;
	auto view = m_camera.generateViewMat();
	auto proj = m_camera.generateProjMat(aspect);
	auto invMat = XMMatrixInverse(nullptr, view * proj);

	float xPoint = (float(x + 0.5f) / m_window->m_windowW) * 2 - 1;
	float yPoint = (float(m_window->m_windowH - y - 0.5f) / m_window->m_windowH) * 2 - 1;
	XMVECTOR nearPoint = XMVector4Transform(XMVectorSet(xPoint, yPoint, 0.0, 1.0), invMat);
	XMVECTOR farPoint = XMVector4Transform(XMVectorSet(xPoint, yPoint, 1.0, 1.0), invMat);
	farPoint /= XMVectorSplatW(farPoint);
	nearPoint /= XMVectorSplatW(nearPoint);
	RayDX ray = RayDX(nearPoint, farPoint - nearPoint);

	rec.reset(m_camera.m_nearPlaneDist);
	MeshSystem::GetInstance().CheckRayIntersections(ray, rec);
}

void Engine::DragObject(MeshIntersection& capturedObject, float deltaX, float deltaY)
{
	XMVECTOR HitVec = capturedObject.pos - m_camera.m_transform.position;

	HitVec.m128_f32[3] = 0;
	float aspectRatio = float(m_window->m_windowW) / m_window->m_windowH;
	float worldDeltaX = 2 * tanf(XMConvertToRadians(m_camera.m_fov / 2)) * m_camera.m_nearPlaneDist * aspectRatio * deltaX / m_window->m_windowW;
	float worldDeltaY = 2 * tanf(XMConvertToRadians(m_camera.m_fov / 2)) * m_camera.m_nearPlaneDist * deltaY / m_window->m_windowH;
	HitVec = XMVector4Transform(HitVec, m_camera.generateViewMat());
	worldDeltaY *= HitVec.m128_f32[2] / m_camera.m_nearPlaneDist;
	worldDeltaX *= HitVec.m128_f32[2] / m_camera.m_nearPlaneDist;

	auto offset = -m_camera.m_transform.Up() * worldDeltaY + m_camera.m_transform.Right() * worldDeltaX;
	capturedObject.model->Move(offset, capturedObject.instance->transformID);
	capturedObject.pos += offset;
}

void Engine::Init()
{
	auto& d3d_singleton = D3D::GetInstance();
	d3d_singleton.Init();

	CONSTANT_BUFFER_PER_VIEW_STRUCT ConstDataPerView;
	CONSTANT_BUFFER_PER_FRAME_STRUCT ConstDataPerFrame;
	CONSTANT_BUFFER_PER_TEXTURE_STRUCT ConstantDataIblGen;
	ConstDataPerView.viewProj = DirectX::XMMATRIX();
	ConstDataPerView.camera_pos = DirectX::XMVECTOR();
	ConstDataPerFrame.EV100 = 0.0f;
	ConstDataPerFrame.g_time = 0;
	ConstDataPerFrame.overwriteRoughness = 0;
	ConstDataPerFrame.overwriteMetalness = 0;
	ConstDataPerFrame.enableDiffuseIrradiance = 0;
	ConstDataPerFrame.enableSpecularIrradiance = 0;
	ConstDataPerFrame.roughnessOverwriteValue = 0.5f;
	ConstDataPerFrame.metalnessOverwriteValue = 0.5f;
	ConstantDataIblGen.roughness = 0.0f;
	auto& renderer = Renderer::GetInstance();
	renderer.InitConstBufferAtSlot(BufferSlots::PER_VIEW, &ConstDataPerView, sizeof(CONSTANT_BUFFER_PER_VIEW_STRUCT));
	renderer.InitConstBufferAtSlot(BufferSlots::ADDITIONAL_PER_VIEW, &ConstDataPerView, sizeof(CONSTANT_BUFFER_ADDITIONAL_PER_VIEW_STRUCT));
	renderer.InitConstBufferAtSlot(BufferSlots::PER_FRAME, &ConstDataPerFrame, sizeof(CONSTANT_BUFFER_PER_FRAME_STRUCT));
	renderer.InitConstBufferAtSlot(BufferSlots::IBL_GEN, &ConstantDataIblGen, sizeof(CONSTANT_BUFFER_PER_TEXTURE_STRUCT));
	renderer.InitSamplerAtSlot(0, pointWrapSampler());
	renderer.InitSamplerAtSlot(1, linearWrapSampler());
	renderer.InitSamplerAtSlot(2, anisotropicWrapSampler());
	renderer.InitSamplerAtSlot(3, shadowComparisonSampler());
	renderer.InitBlendState("opaqueBlend", BlendStateDescOpaque());
	renderer.InitBlendState("translucentOrderedBlend", BlendStateDescOrderedTranslucent());
	m_timer.BeginCounting();
}

void Engine::Run()
{
	auto& renderer = Renderer::GetInstance();
	auto& lightSystem = LightSystem::GetInstance();
	auto& particleSystem = ParticleSystem::GetInstance();
	UpdateBufferWithCameraMatrices();
	auto ConstDataPerFrame = renderer.GetConstBufferData<CONSTANT_BUFFER_PER_FRAME_STRUCT>(BufferSlots::PER_FRAME);

	m_timer.BeginCounting();
	while (true)
	{
		ConstDataPerFrame->g_time = m_timer.GetCounted();
		float deltaTime = m_timer.GetDeltaTime();

		static int frameCounter = 0;
		frameCounter++;
		if (deltaTime >= 0.02 || frameCounter == 60) { std::cout << deltaTime << std::endl; frameCounter = 0; }
		
		renderer.UpdateConstBufferAtSlot<CONSTANT_BUFFER_PER_FRAME_STRUCT>(BufferSlots::PER_FRAME);

		if (!m_window->HandleMessages(m_inputHandlerCallback))
		{
			break;
		}
		ChangeCameraState();
		particleSystem.Update(deltaTime);
		lightSystem.BindLightsSRV();	

		renderer.DrawDeferred();
		m_timer.WaitUntilNextFrame(60);
	}
}

void Engine::DeInit()
{
}

void DeInit()
{
	auto& d3d_singleton = D3D::GetInstance();
	d3d_singleton.DeInit();
}
