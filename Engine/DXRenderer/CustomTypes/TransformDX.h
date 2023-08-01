#pragma once
#include "../Utils/d3d.h"
#include <iostream>

using namespace DirectX;

struct TransformDX
{
	
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR rotation;
	DirectX::XMVECTOR scale;

	static TransformDX IdentityTransform()
	{
		static TransformDX identity;
		identity.position = { 0, 0, 0, 1 };
		identity.rotation = DirectX::XMVectorSet(0, 0, 0, 1);
		identity.scale = { 1, 1, 1, 1 };
		return identity;
	}

	static TransformDX fromMat4(const DirectX::XMMATRIX& matrix)
	{
		TransformDX result;
		DirectX::XMMatrixDecompose(&result.scale, &result.rotation, &result.position, matrix);
		return result;
	}

	DirectX::XMMATRIX ToMat4() const
	{
		return  XMMatrixTransformation(XMVectorZero(),
				XMQuaternionIdentity(),
				scale, 
				XMVectorZero(),
				rotation,                
				position);
	}

	DirectX::XMVECTOR Right()
	{
		auto rotationMat = DirectX::XMMatrixRotationQuaternion(rotation);
		return DirectX::XMVectorSet(rotationMat.r[0].m128_f32[0], rotationMat.r[0].m128_f32[1], rotationMat.r[0].m128_f32[2], 0.0f);
	}
	DirectX::XMVECTOR Up()
	{
		auto rotationMat = DirectX::XMMatrixRotationQuaternion(rotation);
		return DirectX::XMVectorSet(rotationMat.r[1].m128_f32[0], rotationMat.r[1].m128_f32[1], rotationMat.r[1].m128_f32[2], 0.0f);
	}
	DirectX::XMVECTOR Forward()
	{
		auto rotationMat = DirectX::XMMatrixRotationQuaternion(rotation);
		return DirectX::XMVectorSet(rotationMat.r[2].m128_f32[0], rotationMat.r[2].m128_f32[1], rotationMat.r[2].m128_f32[2], 0.0f);
	}
};
