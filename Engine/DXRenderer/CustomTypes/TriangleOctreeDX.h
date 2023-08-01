#pragma once
#include "../Math/BoxDX.h"
#include <vector>
#include <algorithm>
#include <array>
#include "Vertex.h"

template <typename T>
class InstancedMesh;

struct PerInstance;
class Model;

struct MeshIntersection
{
	XMVECTOR pos;
	XMVECTOR normal;
	float near;
	float t;
	uint32_t triangle;
	Model* model;
	InstancedMesh<Vertex>* mesh;
	PerInstance* instance;

	constexpr void reset(float near, float far = std::numeric_limits<float>::infinity())
	{
		this->near = near;
		t = far;
		model = nullptr;
		mesh = nullptr;
		instance = nullptr;
	}
	bool valid() const { return std::isfinite(t); }
};

class TriangleOctreeDX
{
public:
	const static int PREFFERED_TRIANGLE_COUNT;
	const static float MAX_STRETCHING_RATIO;

	void clear() { m_mesh = nullptr; }
	bool inited() const { return m_mesh != nullptr; }

	void initialize(const InstancedMesh<Vertex>& mesh);

	bool intersect(const RayDX& ray, MeshIntersection& nearest) const;

protected:
	const InstancedMesh<Vertex>* m_mesh = nullptr;
	std::vector<uint32_t> m_triangles;

	BoxDX m_box;
	BoxDX m_initialBox;

	std::unique_ptr<std::array<TriangleOctreeDX, 8>> m_children;

	void initialize(const InstancedMesh<Vertex>& mesh, const BoxDX& parentBox, const XMVECTOR& parentCenter, int octetIndex);

	bool addTriangle(uint32_t triangleIndex, const XMVECTOR& V1, const XMVECTOR& V2, const XMVECTOR& V3, const XMVECTOR& center);

	bool intersectInternal(const RayDX& ray, MeshIntersection& nearest) const;
};
