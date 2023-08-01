#pragma once
#include "../Utils/d3d.h"

struct DepthStencilState
{
	DepthStencilState() = default;
	DepthStencilState(D3D11_DEPTH_STENCIL_DESC depthStencilDesc);
	void Bind(UINT referenceValue) const;

	static D3D11_DEPTH_STENCIL_DESC CreateDepthStencilStateDesc(
		bool depthEnable, bool stencilEnable,
		D3D11_DEPTH_WRITE_MASK depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_FUNC depthFunc = D3D11_COMPARISON_GREATER_EQUAL,
		UINT8 stencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK, UINT8 stencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK,
		D3D11_DEPTH_STENCILOP_DESC frontFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS },
		D3D11_DEPTH_STENCILOP_DESC backFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS }
	);

private:
	DxResPtr<ID3D11DepthStencilState> m_depthStencilState;
};

