#include "DepthStencilState.h"

DepthStencilState::DepthStencilState(D3D11_DEPTH_STENCIL_DESC depthStencilDesc)
{
	s_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.reset());
}

void DepthStencilState::Bind(UINT referenceValue) const
{
	s_devcon->OMSetDepthStencilState(m_depthStencilState, referenceValue);
}

D3D11_DEPTH_STENCIL_DESC DepthStencilState::CreateDepthStencilStateDesc(bool depthEnable,
	bool stencilEnable,
	D3D11_DEPTH_WRITE_MASK depthWriteMask,
	D3D11_COMPARISON_FUNC depthFunc,
	UINT8 stencilReadMask,
	UINT8 stencilWriteMask,
	D3D11_DEPTH_STENCILOP_DESC frontFace,
	D3D11_DEPTH_STENCILOP_DESC backFace)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = depthEnable;
	depthStencilDesc.DepthFunc = depthFunc;
	depthStencilDesc.DepthWriteMask = depthWriteMask;
	depthStencilDesc.StencilEnable = stencilEnable;
	depthStencilDesc.StencilReadMask = stencilReadMask;
	depthStencilDesc.StencilWriteMask = stencilWriteMask;
	depthStencilDesc.FrontFace = frontFace;
	depthStencilDesc.BackFace = backFace;
	return depthStencilDesc;
}
