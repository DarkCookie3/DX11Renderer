#pragma once
#include "Ray.h"

struct SpotLight
{
	glm::vec3 m_position;
	glm::vec3 m_direction;
	float m_angle;
    float m_glowRadius;

	bool Hit(const Ray& r, float tMax, HitRecord& rec) const
	{
        glm::vec3 oc = r.m_origin - m_position;
        auto a = glm::length(r.m_direction) * glm::length(r.m_direction);
        auto half_b = glm::dot(oc, r.m_direction);
        auto c = glm::length(oc) * glm::length(oc) - m_glowRadius * m_glowRadius;
        auto discriminant = half_b * half_b - a * c;

        if (discriminant > 0)
        {
            auto root = sqrt(discriminant);
            auto t = (-half_b - root) / a;

            if (t > 0 && t < tMax)
            {
                glm::vec3 point = r.GetCoordinateAt(t);
                glm::vec3 outward_normal = (point - m_position) / m_glowRadius;
                rec.m_normal = outward_normal;
                rec.m_point = point;
                rec.m_t = t;

                return true;
            }
        }
        return false;
	}
};

