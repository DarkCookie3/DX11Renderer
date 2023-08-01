#pragma once
#include "../Utils/d3d.h"
#include "LightMaterial.h"

using namespace DirectX;

struct PackedSpotLight
{
	XMMATRIX lightViewProj;
	XMMATRIX localToWorld;
	XMMATRIX worldToLocal;
	XMVECTOR position;
	XMVECTOR direction;
	XMVECTOR color;
	float lightAngleRad;
	float fadeAngleRad;
	float radius;
	float intensity;
	int texArraySliceIndex;
	float texCoordScale[2];
	int shadowMapIndex;
};

struct SpotLight
{
	TransformDX_ID lightTransform;
	std::shared_ptr<Model> model;
	float lightAngleRad;
	float fadeAngleRad;
	LightMaterial lightMat;

	PackedSpotLight GetPacked();
};