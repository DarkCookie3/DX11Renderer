#pragma once
#include "../../Utils/Material.h"
#include "../../Utils/ObjectRef.h"
#include "../Math/Sphere.h"
#include "../Math/Ray.h"

struct SphereEntity : public Sphere
{
	std::shared_ptr<Material> material;

	bool Hit(const Ray& ray, float tMax, ObjectRef& outRef, HitRecord& rec, std::shared_ptr<Material>& outMaterial)
	{
		bool found = Sphere::Hit(ray, tMax, rec);
		if (found)
		{
			rec.set_normal(ray, rec.m_normal);
			outRef.m_type = IntersectedType::Sphere;
			outRef.m_object = this;
			outMaterial = material;
		}
		return found;
	}
};
