#pragma once
#include "RayDX.h"

struct HitRecordDX
{
    XMVECTOR m_point;
    XMVECTOR m_normal;
    float m_t;

    void set_normal(RayDX ray, XMVECTOR object_normal)
    {
        bool isFrontFace = XMVectorGetX(XMVector3Dot(ray.m_direction, object_normal)) < 0;
        m_normal = isFrontFace ? object_normal : -object_normal;
    }
};