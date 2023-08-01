#pragma once
#include "../../../Submodules/glm/glm/glm.hpp"

struct IObjectMover
{
	virtual void Move(const glm::vec3& offset) = 0;
};