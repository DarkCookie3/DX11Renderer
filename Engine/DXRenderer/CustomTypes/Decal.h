#pragma once
#include "TransformDX.h"

struct Decal
{
	TransformDX m_trasform;
	DxResPtr<ID3D11Texture2D> m_texture;
};