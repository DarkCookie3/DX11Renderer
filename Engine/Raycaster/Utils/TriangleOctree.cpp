#include "TriangleOctree.h"
#include <array>
#include <algorithm>

inline const glm::vec3& getPos(const Mesh& mesh, uint32_t triangleIndex, uint32_t vertexIndex)
{
	glm::vec3 pos = *((glm::vec3*)(&(mesh.m_triangles.get()->at(triangleIndex))) + vertexIndex);

	return pos;
}

inline const Triangle& getTriangle(const Mesh& mesh, uint32_t triangleIndex)
{
	Triangle& triangle = mesh.m_triangles.get()->at(triangleIndex);

	return triangle;
}

void TriangleOctree::Initialize(const Mesh& mesh)
{
	m_mesh = std::make_unique<Mesh>(mesh);
	m_children = nullptr;

	const glm::vec3 eps = { 1e-5f, 1e-5f, 1e-5f };
	m_boundingBox = { mesh.m_box.min - eps, mesh.m_box.max + eps };

	for (uint32_t i = 0; i < mesh.m_triangles->size(); ++i)
	{
		const glm::vec3& V1 = getPos(mesh, i, 0);
		const glm::vec3& V2 = getPos(mesh, i, 1);
		const glm::vec3& V3 = getPos(mesh, i, 2);

		AddTriangle(i, V1, V2, V3);
	}
}

bool TriangleOctree::Hit(const Ray& ray, float tMax, HitRecord& rec) const
{
	if (!m_boundingBox.Hit(ray, tMax, rec))
		return false;

	return HitInternal(ray, tMax, rec);
}

void TriangleOctree::Initialize(const Mesh& mesh, const Box& parentBox, const glm::vec3& parentCenter, int octetIndex)
{
	m_mesh = std::make_unique<Mesh>(mesh);
	m_children = nullptr;

	const float eps = 1e-5f;

	if (octetIndex % 2 == 0)
	{
		m_boundingBox.min[0] = parentBox.min[0];
		m_boundingBox.max[0] = parentCenter[0];
	}
	else
	{
		m_boundingBox.min[0] = parentCenter[0];
		m_boundingBox.max[0] = parentBox.max[0];
	}

	if (octetIndex % 4 < 2)
	{
		m_boundingBox.min[1] = parentBox.min[1];
		m_boundingBox.max[1] = parentCenter[1];
	}
	else
	{
		m_boundingBox.min[1] = parentCenter[1];
		m_boundingBox.max[1] = parentBox.max[1];
	}

	if (octetIndex < 4)
	{
		m_boundingBox.min[2] = parentBox.min[2];
		m_boundingBox.max[2] = parentCenter[2];
	}
	else
	{
		m_boundingBox.min[2] = parentCenter[2];
		m_boundingBox.max[2] = parentBox.max[2];
	}

	glm::vec3 elongation = (MAX_STRETCHING_RATIO - 1.f) * m_boundingBox.GetSize();

	if (octetIndex % 2 == 0) m_boundingBox.max[0] += elongation[0];
	else m_boundingBox.min[0] -= elongation[0];

	if (octetIndex % 4 < 2) m_boundingBox.max[1] += elongation[1];
	else m_boundingBox.min[1] -= elongation[1];

	if (octetIndex < 4) m_boundingBox.max[2] += elongation[2];
	else m_boundingBox.min[2] -= elongation[2];
}

bool TriangleOctree::AddTriangle(uint32_t triangleIndex, const glm::vec3& V1, const glm::vec3& V2, const glm::vec3& V3)
{
	if (!(m_boundingBox.Contains((V1 + V2 + V3) / 3.0f) ||
		m_boundingBox.Contains(V1) ||
		m_boundingBox.Contains(V2) ||
		m_boundingBox.Contains(V3)))
	{
		return false;
	}

	if (m_children == nullptr)
	{
		if (m_triangles.size() <= PREFFERED_TRIANGLE_COUNT)
		{
			m_triangles.emplace_back(triangleIndex);
			return true;
		}
		else
		{
			glm::vec3 C = m_boundingBox.GetCenter();
			m_children = std::make_unique<std::array<TriangleOctree, 8>>(std::array<TriangleOctree, 8>());
			for (int i = 0; i < 8; ++i)
			{
				m_children.get()->at(i).Initialize(*m_mesh, m_boundingBox, C, i);
			}
			for (uint32_t index : m_triangles)
			{
				const glm::vec3& P1 = getPos(*m_mesh, index, 0);
				const glm::vec3& P2 = getPos(*m_mesh, index, 1);
				const glm::vec3& P3 = getPos(*m_mesh, index, 2);

				for (int i = 0; i < 8; ++i)
				{
					m_children.get()->at(i).AddTriangle(index, P1, P2, P3);
				}
			}

			m_triangles.clear();
		}
	}
	else
	{
		for (int i = 0; i < 8; ++i)
		{
			if (m_children.get()->at(i).AddTriangle(triangleIndex, V1, V2, V3))
			{
				break;
			}
		}
	}
	
	return true;
}

bool TriangleOctree::HitInternal(const Ray& ray, float tMax, HitRecord& rec) const
{
	bool found = false;
	float t_max = tMax;
	for (uint32_t i = 0; i < m_triangles.size(); ++i)
	{
		const Triangle& triangle = getTriangle(*m_mesh, m_triangles[i]);
		
		if (triangle.Hit(ray, t_max, rec))
		{
			t_max = rec.m_t;
			found = true;
		}
	}

	if (!m_children) return found;

	struct OctantIntersection
	{
		int index;
		float t;
	};

	std::array<OctantIntersection, 8> boxIntersections;

	for (int i = 0; i < 8; ++i)
	{
		if (m_children.get()->at(i).m_boundingBox.Contains(ray.m_origin))
		{
			boxIntersections[i].index = i;
			boxIntersections[i].t = 0.f;
		}
		else
		{
			float boxT = rec.m_t;
			if (m_children.get()->at(i).m_boundingBox.Hit(ray, t_max, rec))
			{
				boxIntersections[i].index = i;
				boxIntersections[i].t = boxT;
			}
			else
			{
				boxIntersections[i].index = -1;
			}
		}
	}

	std::sort(boxIntersections.begin(), boxIntersections.end(),
		[](const OctantIntersection& A, const OctantIntersection& B) -> bool { return A.t < B.t; });

	for (int i = 0; i < 8; ++i)
	{
		if (boxIntersections[i].index < 0)
			continue;

		if (m_children.get()->at(boxIntersections[i].index).HitInternal(ray, t_max, rec))
		{
			t_max = rec.m_t;
			found = true;
		}
	}

	return found;
}
