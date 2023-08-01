#pragma once
#include "../Utils/d3d.h"

struct RasterizerState
{
	RasterizerState() = default;
	RasterizerState(D3D11_RASTERIZER_DESC rasterizerState);
	void Bind() const;

	static D3D11_RASTERIZER_DESC CreateRasterizerStateDesc(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode, float depthBias, float depthBiasSlope, bool multisample);
private:
	DxResPtr<ID3D11RasterizerState> m_rasterizerState;
};