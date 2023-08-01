#pragma once
#include "../Utils/d3d.h"
#include "LightMaterial.h"

using namespace DirectX;

struct PackedPointLight
{
	XMMATRIX lightViewProj[6];
	XMVECTOR position;
	XMVECTOR color;
	float radius;
	float intensity;
	int shadowMapIndex;
	float padding;
};

struct PointLight
{
	TransformDX_ID lightTransform;
	std::shared_ptr<Model> model;
	LightMaterial lightMat;

	PackedPointLight GetPacked();
};