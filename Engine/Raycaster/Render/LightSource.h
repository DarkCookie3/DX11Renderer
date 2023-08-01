#pragma once
#include "../../Submodules/glm/glm/glm.hpp"

struct LightSource
{
	LightSource(glm::vec3 position, float strength, uint32_t color)
		: m_position(position), m_strength(strength), m_color(color)
	{
	}

	glm::vec3 m_position;
	float m_strength;
	uint32_t m_color;
};