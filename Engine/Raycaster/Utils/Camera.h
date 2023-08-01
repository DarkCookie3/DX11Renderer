#pragma once
#include "../../Submodules/glm/glm/glm.hpp"
#include "../../Submodules/glm/glm/gtc/matrix_transform.hpp"
#include "Transform.h"

struct Camera
{
	Camera() = default;
	Camera(glm::vec3 position, glm::vec3 lookAt, float nearPlaneDist, float farPlaneDist, float fov, float aspect)
		: m_nearPlaneDist(nearPlaneDist), m_farPlaneDist(farPlaneDist), m_fov(fov)
	{
		glm::vec3 forward = glm::normalize(lookAt - position);
		glm::quat quatRotation = glm::quatLookAtLH(forward, glm::vec3(0, 1, 0));

		m_transform = {position, quatRotation, glm::vec3(1.0f)};
	}

	glm::mat4 generateViewMat() const { return glm::inverse(m_transform.ToMat4()); }
	glm::mat4 generateProjMat(float aspect) const { return glm::perspectiveLH(glm::radians(m_fov), aspect, m_nearPlaneDist, m_farPlaneDist); }

	void Move(const glm::vec3& delta)
	{
		m_transform.position += delta;
	}
	void Rotate(float rollDelta, float pitchDelta, float yawDelta)
	{
		m_transform.rotation = glm::quat(glm::vec3(glm::radians(pitchDelta), glm::radians(yawDelta), glm::radians(rollDelta))) * m_transform.rotation;
	}
	void Rotate(float angle, const glm::vec3& axis)
	{
		m_transform.rotation *= glm::angleAxis(glm::radians(angle), axis);
	}
	void SetRotation(float roll, float pitch, float yaw)
	{
		m_transform.rotation = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll)));
	}
	
	Transform m_transform;
	float m_nearPlaneDist;
	float m_farPlaneDist;
	float m_fov;
};