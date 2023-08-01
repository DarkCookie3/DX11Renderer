#pragma once
#include "../Math/Triangle.h"
#include "IObjectMover.h"

class TriangleMover : public IObjectMover
{
public:
	virtual void Move(const glm::vec3& offset) override
	{
		triangle->m_vertexA += offset;
		triangle->m_vertexB += offset;
		triangle->m_vertexC += offset;
	}
	Triangle* triangle;
};