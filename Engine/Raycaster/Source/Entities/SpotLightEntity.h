#pragma once
#include "../../Utils/LightMat.h"
#include "../Math/SpotLight.h"

struct SpotLightEntity : public SpotLight
{
	std::shared_ptr<LightMat> m_light;

	bool Hit(const Ray& ray, float tMax, ObjectRef& outRef, HitRecord& rec, std::shared_ptr<LightMat>& outMaterial)
	{
		bool found = SpotLight::Hit(ray, tMax, rec);
		if (found)
		{
			outRef.m_type = IntersectedType::SpotLight;
			outRef.m_object = this;
			outMaterial = m_light;
		}
		return found;
	}
};