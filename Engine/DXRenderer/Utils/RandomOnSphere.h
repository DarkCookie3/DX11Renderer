#pragma once
#include <DirectXMath.h>
#include <stdlib.h>

using namespace DirectX;

XMVECTOR RandomOnUnitSphere()
{
	XMVECTOR result;

	float theta = 2 * 3.141 * ((float)rand() / RAND_MAX);
	float phi = acos(2.0f * ((float)rand() / RAND_MAX) - 1.0f);

	result.m128_f32[0] = sin(phi) * cos(theta);
	result.m128_f32[1] = sin(phi) * sin(theta);
	result.m128_f32[2] = cos(phi);

	return result;
}