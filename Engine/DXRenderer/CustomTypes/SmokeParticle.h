#pragma once
#include "../Utils/d3d.h"
#include <array>

using namespace DirectX;

constexpr float maxSmokeParticleSize = 0.8f;
constexpr float minSmokeParticleSize = 0.1f;
constexpr float smokeParticleAnimationRate = 1.0f;
constexpr float smokeParticleVerticalAcceleration = 1.0f;

struct PackedSmokeParticle
{
	XMMATRIX modelMat;
	XMVECTOR colorRGBA;
	XMVECTOR emissionColor;
	float animationState;
};

struct SmokeParticle
{
	XMVECTOR colorRGBA;
	XMVECTOR emissionColor;
	XMVECTOR position;
	XMVECTOR velocity;
	float timeAlive;
	float animationState;
	float rotation;
	std::array<float, 2> size;

	PackedSmokeParticle GetPacked()
	{
		PackedSmokeParticle result;
		result.colorRGBA = colorRGBA;
		result.emissionColor = emissionColor;
		result.modelMat = XMMatrixTransformation(XMVectorZero(),
			XMQuaternionIdentity(),
			XMVectorSet(size[0], size[1], 1, 1),
			XMVectorZero(),
			XMQuaternionRotationRollPitchYaw(0, 0, rotation),
			position);
		result.animationState = animationState;
		return result;
	}
};