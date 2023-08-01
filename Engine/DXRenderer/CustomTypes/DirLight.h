#pragma once
#include "../Utils/d3d.h"
#include "LightMaterial.h"

using namespace DirectX;

struct PackedDirLight
{
	XMMATRIX lightViewProj;
	XMVECTOR direction;
	XMVECTOR color;
	float intensity;
	int shadowMapIndex;
	float padding[2];
};

struct DirLight
{
	TransformDX_ID lightTransform;
	LightMaterial lightMat;

	PackedDirLight GetPacked();
};