#pragma once
#include "Model.h"

struct LightMaterial
{
	std::shared_ptr<Material> material;
	float intensity;
};