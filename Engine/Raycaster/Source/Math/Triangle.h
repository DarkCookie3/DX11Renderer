#pragma once
#include "../../Utils/HitRecord.h"

struct Triangle
{
    glm::vec3 m_vertexA;
    glm::vec3 m_vertexB;
    glm::vec3 m_vertexC;

    bool Hit(const Ray& r, float t_max, HitRecord& rec) const
    {
        float EPSILON = 0.0001f;

        glm::vec3 edge1 = m_vertexB - m_vertexA;
        glm::vec3 edge2 = m_vertexC - m_vertexA;
        glm::vec3 h = glm::cross(r.m_direction, edge2);
        float a = glm::dot(edge1, h);
        if (a > -EPSILON && a < EPSILON)
        {
            return false;
        }
        float f = 1.0 / a;
        glm::vec3 s = r.m_origin - m_vertexA;
        float u = f * glm::dot(s, h);
        if (u < 0.0 || u > 1.0)
        {
            return false;
        }
        glm::vec3 q = glm::cross(s, edge1);
        float v = f * glm::dot(r.m_direction, q);
        if (v < 0.0 || u + v > 1.0) 
        {
            return false;
        }
        float t = f * glm::dot(edge2, q);
        if (t < t_max && t > 0)
        {
            rec.m_normal = glm::normalize(glm::cross(edge1, edge2));
            rec.m_point = r.GetCoordinateAt(t);
            rec.m_t = t;
            return true;
        }
        else
        {
            return false;
        }
    }
};