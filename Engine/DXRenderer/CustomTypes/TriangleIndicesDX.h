#pragma once
#include "Vertex.h"
#include "../Math/RayDX.h"
#include <directxcollision.h>
#include "../Math/HitRecordDX.h"

using namespace DirectX;

struct TriangleIndicesDX
{
	uint32_t indices[3];
};

static bool IntersectTriangle(const RayDX& r, const XMVECTOR& V1, const XMVECTOR& V2, const XMVECTOR& V3, HitRecordDX& rec, float t_max)
{
	float t;

	auto normalDir = XMVector3Normalize(r.m_direction);
	if (TriangleTests::Intersects(r.m_origin, normalDir, V1, V2, V3, t))
	{
		t *= XMVector3Length(normalDir).m128_f32[0] / XMVector3Length(r.m_direction).m128_f32[0];
		if (t > 0 && t < t_max)
		{
			rec.set_normal(r, XMVector3Normalize(XMVector3Cross(V1-V2, V2-V3)));
			rec.m_point = r.GetCoordinateAt(t);
			rec.m_t = t;
			return true;
		}
	}
	return false;
}