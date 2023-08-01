#pragma once

#include "HitRecordDX.h"
#include <directxcollision.h>

struct BoxDX
{
	XMVECTOR min;
	XMVECTOR max;

	XMVECTOR GetCenter() const
	{
		return (min + max) / 2.0f;
	}
	XMVECTOR GetExtents() const
	{
		return (max - min) / 2.0f;
	}
	bool Contains(const XMVECTOR& v)
	{
		if (XMVectorGetX(v) <= XMVectorGetX(max) && XMVectorGetX(v) >= XMVectorGetX(min) && XMVectorGetY(v) <= XMVectorGetY(max) && XMVectorGetY(v) >= XMVectorGetY(min) && XMVectorGetZ(v) <= XMVectorGetZ(max) && XMVectorGetZ(v) >= XMVectorGetZ(min))
		{
			return true;
		}
		return false;
	}
	bool Hit(const RayDX& ray, float t_max, HitRecordDX& rec) const
	{
		auto center = GetCenter();
		auto extents = GetExtents();
		auto Box = DirectX::BoundingBox({ XMVectorGetX(center), XMVectorGetY(center), XMVectorGetZ(center) }, { XMVectorGetX(extents), XMVectorGetY(extents), XMVectorGetZ(extents) } );
	
		float boxT;
		auto normalDir = XMVector3Normalize(ray.m_direction);
		if (Box.Intersects(ray.m_origin, normalDir, boxT))
		{
			boxT *= XMVector3Length(normalDir).m128_f32[0] / XMVector3Length(ray.m_direction).m128_f32[0];
			if (boxT < t_max)
			{
				rec.m_t = boxT;
				return true;
			}
		}
		return false;
	}
};