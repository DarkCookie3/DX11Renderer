#pragma once
#include "../Math/Plane.h"
#include "IObjectMover.h"

class PlaneMover : public IObjectMover
{
public:
	virtual void Move(const glm::vec3& offset) override
	{
		plane->m_point += offset;
	}
	Plane* plane;
};