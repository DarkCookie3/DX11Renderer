#include "BlendState.h"


BlendState::BlendState(D3D11_BLEND_DESC blendDesc, std::array<float, 4> blendFactor, UINT sampleMask) : m_blendFactor(blendFactor), m_sampleMask(sampleMask)
{
	s_device->CreateBlendState(&blendDesc, m_blendState.reset());
}

void BlendState::Bind()
{
	s_devcon->OMSetBlendState(m_blendState, m_blendFactor.data(), m_sampleMask);
}
