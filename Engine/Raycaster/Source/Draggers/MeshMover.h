#pragma once
#include "../Math/Mesh.h"
#include "IObjectMover.h"

class MeshMover : public IObjectMover
{
public:
	virtual void Move(const glm::vec3& offset) override
	{
		mesh->transform.position += offset;
	}
	Mesh* mesh;
};