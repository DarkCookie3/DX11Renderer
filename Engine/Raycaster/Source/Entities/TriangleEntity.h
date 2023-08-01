#pragma once

#pragma once
#include "../../Utils/Material.h"
#include "../../Utils/ObjectRef.h"
#include "../Math/Triangle.h"
#include "../Math/Ray.h"

struct TriangleEntity : public Triangle
{
	std::shared_ptr<Material> material;

	bool Hit(const Ray& ray, float tMax, ObjectRef& outRef, HitRecord& rec, std::shared_ptr<Material>& outMaterial)
	{
		bool found = Triangle::Hit(ray, tMax, rec);
		if (found)
		{
			rec.set_normal(ray, rec.m_normal);
			outRef.m_type = IntersectedType::Triangle;
			outRef.m_object = this;
			outMaterial = material;
		}
		return found;
	}
};