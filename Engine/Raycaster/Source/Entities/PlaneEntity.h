#pragma once
#include "../../Utils/Material.h"
#include "../../Utils/ObjectRef.h"
#include "../Math/Plane.h"
#include "../Math/Ray.h"

struct PlaneEntity : public Plane
{
	std::shared_ptr<Material> material;

	bool Hit(const Ray& ray, float tMax, ObjectRef& outRef, HitRecord& rec, std::shared_ptr<Material>& outMaterial)
	{
		bool found = Plane::Hit(ray, tMax, rec);
		if (found)
		{
			rec.set_normal(ray, rec.m_normal);
			outRef.m_type = IntersectedType::Plane;
			outRef.m_object = this;
			outMaterial = material;
		}
		return found;
	}
};