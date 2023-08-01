#include "SamplerState.h"

SamplerState::SamplerState(D3D11_SAMPLER_DESC samplerDesc)
{
	s_device->CreateSamplerState(&samplerDesc, m_sampler.reset());
}

void SamplerState::Bind(uint32_t slot)
{
	auto* const samplerPtr = m_sampler.ptr();
	s_devcon->VSSetSamplers(slot, 1, &samplerPtr);
	s_devcon->HSSetSamplers(slot, 1, &samplerPtr);
	s_devcon->DSSetSamplers(slot, 1, &samplerPtr);
	s_devcon->GSSetSamplers(slot, 1, &samplerPtr);
	s_devcon->PSSetSamplers(slot, 1, &samplerPtr);
}
