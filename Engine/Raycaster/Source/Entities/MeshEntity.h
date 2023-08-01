#pragma once
#include "../../Utils/TriangleOctree.h"
#include "../../Utils/Material.h"
#include "../../Utils/ObjectRef.h"

struct MeshEntity : public TriangleOctree
{
	std::shared_ptr<Material> material;

	bool Hit(const Ray& ray, float tMax, ObjectRef& outRef, HitRecord& rec, std::shared_ptr<Material>& outMaterial)
	{
		auto transform = m_mesh.get()->transform.ToMat4();
		auto inv = glm::inverse(transform);
		auto newOrig = inv * glm::vec4(ray.m_origin, 1);
		auto newDir = inv * glm::vec4(ray.m_direction, 0);
		Ray newRay;
		newRay.m_direction = newDir;
		newRay.m_origin = newOrig;
		bool found = TriangleOctree::Hit(newRay, tMax, rec);
		if (found)
		{
			rec.set_normal(newRay, rec.m_normal);
			rec.m_point = transform * glm::vec4(rec.m_point, 1);
			rec.m_normal = inv * glm::vec4(rec.m_normal, 0);
			outRef.m_type = IntersectedType::Mesh;
			outRef.m_object = this;
			outMaterial = material;
		}
		return found;
	}
};