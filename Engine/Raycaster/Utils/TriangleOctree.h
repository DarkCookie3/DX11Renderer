#pragma once
#include <utility>
#include <memory>
#include <array>
#include "../Source/Math/Mesh.h"

struct TriangleOctree
{
public:
	constexpr static int PREFFERED_TRIANGLE_COUNT = 16;
	constexpr static float MAX_STRETCHING_RATIO = 1.05f;

	void Initialize(const Mesh& mesh);
	bool Hit(const Ray& ray, float tMax, HitRecord& rec) const;

	void Initialize(const Mesh& mesh, const Box& parentBox, const glm::vec3& parentCenter, int octetIndex);
	bool AddTriangle(uint32_t triangleIndex, const glm::vec3& V1, const glm::vec3& V2, const glm::vec3& V3);
	bool HitInternal(const Ray& ray, float tMax, HitRecord& rec) const;

	std::unique_ptr<std::array<TriangleOctree, 8>> m_children;
	std::vector<uint32_t> m_triangles;
	std::unique_ptr<Mesh> m_mesh = nullptr;
	Box m_boundingBox;
};

