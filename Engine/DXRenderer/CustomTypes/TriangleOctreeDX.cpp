#include "InstancedMesh.h"
#include "TriangleOctreeDX.h"

using namespace DirectX;

const int TriangleOctreeDX::PREFFERED_TRIANGLE_COUNT = 32;
const float TriangleOctreeDX::MAX_STRETCHING_RATIO = 2.f;


void TriangleOctreeDX::initialize(const InstancedMesh<Vertex>& mesh)
{
	m_triangles.clear();
	m_triangles.shrink_to_fit();

	m_mesh = &mesh;
	m_children = nullptr;

	const XMVECTOR eps = { 1e-5f, 1e-5f, 1e-5f };
	m_box = m_initialBox = { mesh.m_boundingBox.min - eps, mesh.m_boundingBox.max + eps };

	for (uint32_t i = 0; i < mesh.m_triangles.size(); ++i)
	{
		const XMVECTOR& V1 = mesh.m_vertices[mesh.m_triangles[i].indices[0]].Position;
		const XMVECTOR& V2 = mesh.m_vertices[mesh.m_triangles[i].indices[1]].Position;
		const XMVECTOR& V3 = mesh.m_vertices[mesh.m_triangles[i].indices[2]].Position;

		XMVECTOR P = (V1 + V2 + V3) / 3.f;

		addTriangle(i, V1, V2, V3, P);
	}
}

void TriangleOctreeDX::initialize(const InstancedMesh<Vertex>& mesh, const BoxDX& parentBox, const XMVECTOR& parentCenter, int octetIndex)
{
	m_mesh = &mesh;
	m_children = nullptr;

	const float eps = 1e-5f;

	if (octetIndex % 2 == 0)
	{
		m_initialBox.min.m128_f32[0] = parentBox.min.m128_f32[0];
		m_initialBox.max.m128_f32[0] = parentCenter.m128_f32[0];
	}
	else
	{
		m_initialBox.min.m128_f32[0] = parentCenter.m128_f32[0];
		m_initialBox.max.m128_f32[0] = parentBox.max.m128_f32[0];
	}

	if (octetIndex % 4 < 2)
	{
		m_initialBox.min.m128_f32[1] = parentBox.min.m128_f32[1];
		m_initialBox.max.m128_f32[1] = parentCenter.m128_f32[1];
	}
	else
	{
		m_initialBox.min.m128_f32[1] = parentCenter.m128_f32[1];
		m_initialBox.max.m128_f32[1] = parentBox.max.m128_f32[1];
	}

	if (octetIndex < 4)
	{
		m_initialBox.min.m128_f32[2] = parentBox.min.m128_f32[2];
		m_initialBox.max.m128_f32[2] = parentCenter.m128_f32[2];
	}
	else
	{
		m_initialBox.min.m128_f32[2] = parentCenter.m128_f32[2];
		m_initialBox.max.m128_f32[2] = parentBox.max.m128_f32[2];
	}
	
	m_box = m_initialBox;
	XMVECTOR elongation = (MAX_STRETCHING_RATIO - 1.f) * 2 * m_box.GetExtents();

	if (octetIndex % 2 == 0) m_box.max.m128_f32[0] -= elongation.m128_f32[0];
	else m_box.min.m128_f32[0] += elongation.m128_f32[0];

	if (octetIndex % 4 < 2) m_box.max.m128_f32[1] -= elongation.m128_f32[1];
	else m_box.min.m128_f32[1] += elongation.m128_f32[1];

	if (octetIndex < 4) m_box.max.m128_f32[2] -= elongation.m128_f32[2];
	else m_box.min.m128_f32[2] += elongation.m128_f32[2];
}

bool TriangleOctreeDX::addTriangle(uint32_t triangleIndex, const XMVECTOR& V1, const XMVECTOR& V2, const XMVECTOR& V3, const XMVECTOR& center)
{
	if (!m_initialBox.Contains(center) ||
		!m_box.Contains(V1) ||
		!m_box.Contains(V2) ||
		!m_box.Contains(V3))
	{
		return false;
	}

	if (m_children == nullptr)
	{
		if (m_triangles.size() < PREFFERED_TRIANGLE_COUNT)
		{
			m_triangles.emplace_back(triangleIndex);
			return true;
		}
		else
		{
			XMVECTOR C = (m_initialBox.min + m_initialBox.max) / 2.f;

			m_children.reset(new std::array<TriangleOctreeDX, 8>());
			for (int i = 0; i < 8; ++i)
			{
				(*m_children)[i].initialize(*m_mesh, m_initialBox, C, i);
			}

			std::vector<uint32_t> newTriangles;

			for (uint32_t index : m_triangles)
			{
				const XMVECTOR& P1 = m_mesh->m_vertices[m_mesh->m_triangles[index].indices[0]].Position;
				const XMVECTOR& P2 = m_mesh->m_vertices[m_mesh->m_triangles[index].indices[1]].Position;
				const XMVECTOR& P3 = m_mesh->m_vertices[m_mesh->m_triangles[index].indices[2]].Position;

				XMVECTOR P = (P1 + P2 + P3) / 3.f;

				int i = 0;
				for (; i < 8; ++i)
				{
					if ((*m_children)[i].addTriangle(index, P1, P2, P3, P))
						break;
				}

				if (i == 8)
					newTriangles.emplace_back(index);
			}

			m_triangles = std::move(newTriangles);
		}
	}

	int i = 0;
	for (; i < 8; ++i)
	{
		if ((*m_children)[i].addTriangle(triangleIndex, V1, V2, V3, center))
			break;
	}

	if (i == 8)
		m_triangles.emplace_back(triangleIndex);

	return true;
}

bool TriangleOctreeDX::intersect(const RayDX& ray, MeshIntersection& nearest) const
{
	if (!inited()) { return false; }

	HitRecordDX rec;
	float boxT = nearest.t;
	if (!m_box.Hit(ray, boxT, rec))
		return false;

	return intersectInternal(ray, nearest);
}

bool TriangleOctreeDX::intersectInternal(const RayDX& ray, MeshIntersection& nearest) const
{
	bool found = false;

	HitRecordDX rec;
	for (uint32_t i = 0; i < m_triangles.size(); ++i)
	{
		const XMVECTOR& V1 = m_mesh->m_vertices[m_mesh->m_triangles[i].indices[0]].Position;
		const XMVECTOR& V2 = m_mesh->m_vertices[m_mesh->m_triangles[i].indices[1]].Position;
		const XMVECTOR& V3 = m_mesh->m_vertices[m_mesh->m_triangles[i].indices[2]].Position;

		if (IntersectTriangle(ray, V1, V2, V3, rec, nearest.t))
		{
			nearest.triangle = i;
			nearest.t = rec.m_t;
			nearest.normal = rec.m_normal;
			nearest.pos = rec.m_point;
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
		if ((*m_children)[i].m_box.Contains(ray.m_origin))
		{
			boxIntersections[i].index = i;
			boxIntersections[i].t = 0.f;
		}
		else
		{
			float boxT = nearest.t;
			if ((*m_children)[i].m_box.Hit(ray, boxT, rec))
			{
				boxIntersections[i].index = i;
				boxIntersections[i].t = rec.m_t;
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
		if (boxIntersections[i].index < 0 || boxIntersections[i].t > nearest.t)
			continue;

		if ((*m_children)[boxIntersections[i].index].intersect(ray, nearest))
		{
			found = true;
		}
	}

	return found;
}
