#pragma once
#include "../Utils/d3d.h"

struct Vertex
{
	DirectX::XMVECTOR Position;
	DirectX::XMVECTOR Normal;
	DirectX::XMVECTOR Tangent;
	DirectX::XMVECTOR Bitangent;
	DirectX::XMVECTOR TexCoords;
};