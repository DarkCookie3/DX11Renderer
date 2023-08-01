#pragma once
#include "../Utils/d3d.h"

using namespace DirectX;

struct RayDX
{
	RayDX() = default;

	RayDX(const DirectX::XMVECTOR& origin, const DirectX::XMVECTOR& direction) : m_origin(origin), m_direction(DirectX::XMVector3Normalize(direction))
	{
	}

	DirectX::XMVECTOR GetCoordinateAt(float t) const
	{
		auto result = m_origin + (t * m_direction);
		return result;
	}

	DirectX::XMVECTOR m_origin;
	DirectX::XMVECTOR m_direction;
};