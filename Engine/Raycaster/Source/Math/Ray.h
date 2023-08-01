#pragma once
#include "../../../Submodules/glm/glm/glm.hpp"

struct Ray
{
	Ray() = default;

	Ray(const glm::vec3& origin, const glm::vec3& direction) : m_origin(origin), m_direction(glm::normalize(direction))
	{
	}

	glm::vec3 GetCoordinateAt(float t) const
	{
		return m_origin + t * m_direction;
	}

	glm::vec3 m_origin;
	glm::vec3 m_direction;
};