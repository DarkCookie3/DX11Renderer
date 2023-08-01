#include "SpotLight.h"

PackedSpotLight SpotLight::GetPacked()
{
	PackedSpotLight result;
	auto modelPos = model->GetTransform().position;
	result.position = lightTransform.GetTransform().position + modelPos;
	result.position.m128_f32[3] = 1;
	auto noScaleModelTransform = XMMatrixRotationQuaternion(model->GetTransform().rotation) * XMMatrixTranslation(modelPos.m128_f32[0], modelPos.m128_f32[1], modelPos.m128_f32[2]);
	result.localToWorld = lightTransform.GetTransform().ToMat4() * noScaleModelTransform;
	result.worldToLocal = XMMatrixInverse(0, result.localToWorld);
	result.intensity = lightMat.intensity;
	result.color = lightMat.material->emissionColor;
	result.direction = XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), lightTransform.GetTransform().rotation);
	result.lightAngleRad = lightAngleRad;
	result.fadeAngleRad = fadeAngleRad;
	result.radius = 1.0f * model->GetTransform().scale.m128_f32[0];
	return result;
}
