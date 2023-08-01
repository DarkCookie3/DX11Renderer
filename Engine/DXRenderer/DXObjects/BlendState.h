#pragma once
#include <cstdint>
#include <array>
#include "../Utils/d3d.h"

class BlendState
{
public:
	BlendState() = default;
	BlendState(D3D11_BLEND_DESC blendDesc, std::array<float, 4> blendFactor, UINT samplemask);
	void Bind();
private:
	DxResPtr<ID3D11BlendState> m_blendState;
	std::array<float, 4> m_blendFactor;
	UINT m_sampleMask;
};


inline D3D11_BLEND_DESC BlendStateDescOpaque()
{
	D3D11_BLEND_DESC blendState;
	ZeroMemory(&blendState, sizeof(D3D11_BLEND_DESC));

	blendState.RenderTarget[0].BlendEnable = FALSE;
	blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	return blendState;
}

inline D3D11_BLEND_DESC BlendStateDescOrderedTranslucent()
{
	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	return blendStateDesc;
}