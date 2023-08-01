#pragma once
#include "../../../Submodules/glm/glm/glm.hpp"
#include "Ray.h"
#include "../../Utils/HitRecord.h"

struct Plane
{
	glm::vec3 m_normal;
    glm::vec3 m_point;

    bool Hit(const Ray& r, float tMax, HitRecord& rec) const
    {
        float denom = glm::dot(m_normal, r.m_direction);
        if (abs(denom) > 0.0001f) 
        {
            float t = glm::dot(m_point - r.m_origin, m_normal) / denom;
            
            if (t >= 0 && t < tMax)
            {
                rec.m_point = r.GetCoordinateAt(t);
                rec.m_normal = m_normal;
                rec.m_t = t;
                return true;
            }
        }
        return false;
    }
};