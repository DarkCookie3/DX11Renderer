#pragma once
#include <cstdint>
#include "../../Utils/LightMat.h"
#include "../Math/DirLight.h"

struct DirLightEntity : public DirLight
{
	std::shared_ptr<LightMat> m_light;

};