#include "PointLight.h"

PackedPointLight PointLight::GetPacked()
{
	PackedPointLight result;
	result.intensity = lightMat.intensity;
	result.color = lightMat.material->emissionColor;
	result.radius = 1.0f * model->GetTransform().scale.m128_f32[0];
	auto modelPos = model->GetTransform().position;
	result.position = lightTransform.GetTransform().position + modelPos;
	result.position.m128_f32[3] = 1;
	return result;
}
