#pragma once
#include "../../Utils/LightMat.h"
#include "../Math/PointLight.h"

struct PointLightEntity : public PointLight
{
	std::shared_ptr<LightMat> m_light;

	bool Hit(const Ray& ray, float tMax, ObjectRef& outRef, HitRecord& rec, std::shared_ptr<LightMat>& outMaterial)
	{
		bool found = PointLight::Hit(ray, tMax, rec);
		if (found)
		{
			outRef.m_type = IntersectedType::PointLight;
			outRef.m_object = this;
			outMaterial = m_light;
		}
		return found;
	}
};