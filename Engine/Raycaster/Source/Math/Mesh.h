#pragma once
#include "Triangle.h"
#include "../../Utils/Transform.h"
#include "../../Utils/Box.h"
#include <vector>

struct Mesh
{
	std::shared_ptr<std::vector<Triangle>> m_triangles;
	Transform transform;
	Box m_box;
};