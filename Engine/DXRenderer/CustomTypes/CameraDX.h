#pragma once
#include "TransformDX.h"

struct CameraDX
{
	CameraDX() = default;
	CameraDX(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt, float nearPlaneDist, float farPlaneDist, float fov)
		: m_nearPlaneDist(nearPlaneDist), m_farPlaneDist(farPlaneDist), m_fov(fov)
	{
		auto lookAtMat = DirectX::XMMatrixLookAtLH(position, lookAt, DirectX::XMVectorSet(0, 1, 0, 0));
		auto lookAtQuat = DirectX::XMQuaternionRotationMatrix(lookAtMat);

		m_transform = { position, lookAtQuat, DirectX::XMVectorSet(1, 1, 1, 1) };
	}

	DirectX::XMMATRIX generateViewMat() const { return DirectX::XMMatrixInverse(nullptr, m_transform.ToMat4()); }
	DirectX::XMMATRIX generateProjMat(float aspect) const { return DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_fov), aspect, m_nearPlaneDist, m_farPlaneDist); }
	DirectX::XMMATRIX generateProjMatReversedZ(float aspect) const 
	{ 
		auto result = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_fov), aspect, m_farPlaneDist, m_nearPlaneDist);
		return result;
	}
	
	void Move(const DirectX::XMVECTOR& delta)
	{
		m_transform.position = DirectX::XMVectorAdd(m_transform.position, delta);
	}
	void Rotate(float rollDelta, float pitchDelta, float yawDelta)
	{
		m_transform.rotation = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(pitchDelta), DirectX::XMConvertToRadians(yawDelta), DirectX::XMConvertToRadians(rollDelta)), m_transform.rotation);
	}
	void Rotate(float angle, const DirectX::XMVECTOR& axis)
	{
		m_transform.rotation = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionRotationAxis(axis, DirectX::XMConvertToRadians(angle)), m_transform.rotation);
	}
	void RotateByWorld(float angle, const DirectX::XMVECTOR& axis)
	{
		m_transform.rotation = DirectX::XMQuaternionMultiply(m_transform.rotation, DirectX::XMQuaternionRotationAxis(axis, DirectX::XMConvertToRadians(angle)));
	}
	void SetRotation(float roll, float pitch, float yaw)
	{
		m_transform.rotation = DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));
	}

	TransformDX m_transform;
	float m_nearPlaneDist;
	float m_farPlaneDist;
	float m_fov;
};