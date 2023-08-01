#pragma once
#include "../../Submodules/glm/glm/glm.hpp"
#include "../Utils/HitRecord.h"

struct Box
{
	glm::vec3 min;
	glm::vec3 max;

	glm::vec3 GetCenter()
	{
		return (min + max) / 2.0f;
	}
	glm::vec3 GetSize()
	{
		return (max - min) / 2.0f;
	}
	bool Contains(const glm::vec3& v)
	{
		if (v.x <= max.x && v.x >= min.x && v.y <= max.y && v.y >= min.y && v.z <= max.z && v.z >= min.z)
		{
			return true;
		}
		return false;
	}
	bool Hit(const Ray& ray, float t_max, HitRecord& rec) const
	{
		glm::vec3 tMin = (min - ray.m_origin) / ray.m_direction;
		glm::vec3 tMax = (max - ray.m_origin) / ray.m_direction;
		glm::vec3 t1 = glm::min(tMin, tMax);
		glm::vec3 t2 = glm::max(tMin, tMax);
		float tNear = std::max(std::max(t1.x, t1.y), t1.z);
		float tFar = std::min(std::min(t2.x, t2.y), t2.z);

		if (tNear > tFar || t_max < tNear || tNear < 0)
		{
			if (tFar < 0 || t_max < tNear)
			{
				return false;
			}
			else
			{
				rec.m_t = tFar;	
				return true;
			}
			return false;
		}
		else
		{
			rec.m_t = tNear;
			return true;
		}
	}
};