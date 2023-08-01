#include "DirLight.h"

PackedDirLight DirLight::GetPacked()
{
	PackedDirLight result;
	result.intensity = lightMat.intensity;
	result.color = lightMat.material->emissionColor;
	result.direction = XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), lightTransform.GetTransform().rotation);
	return result;
}
