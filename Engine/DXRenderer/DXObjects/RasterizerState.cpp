#include "RasterizerState.h"

RasterizerState::RasterizerState(D3D11_RASTERIZER_DESC rasterizerState)
{
	s_device->CreateRasterizerState(&rasterizerState, m_rasterizerState.reset());
}

void RasterizerState::Bind() const
{
	s_devcon->RSSetState(m_rasterizerState);
}

D3D11_RASTERIZER_DESC RasterizerState::CreateRasterizerStateDesc(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode, float depthBias, float depthBiasSlope, bool multisample)
{
	D3D11_RASTERIZER_DESC rasterizerState;
	rasterizerState.FillMode = fillMode;
	rasterizerState.CullMode = cullMode;
	rasterizerState.FrontCounterClockwise = false;
	rasterizerState.DepthBias = depthBias;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = depthBiasSlope;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = multisample;
	rasterizerState.AntialiasedLineEnable = false;
	return rasterizerState;
}
