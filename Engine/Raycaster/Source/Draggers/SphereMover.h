#pragma once
#include "../Math/Sphere.h"
#include "IObjectMover.h"
	
class SphereMover : public IObjectMover
{
public:
	virtual void Move(const glm::vec3& offset) override
	{
		sphere->m_center += offset;
	}
	Sphere* sphere;
};
