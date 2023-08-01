#pragma once
#include "../../Submodules/glm/glm/glm.hpp"
#include "../Source/Math/Ray.h"
#include <memory>

struct HitRecord
{
    glm::vec3 m_point;
    glm::vec3 m_normal;
    float m_t;

    void set_normal(Ray ray, glm::vec3 object_normal)
    {
        bool isFrontFace = glm::dot(ray.m_direction, object_normal) < 0;
        m_normal = isFrontFace ? object_normal : -object_normal;
    }
};