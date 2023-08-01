#include "StructuredBuffer.h"

StructuredBuffer::~StructuredBuffer()
{
}

void StructuredBuffer::Bind(uint32_t srvSlot)
{
	auto const srv = m_srv.ptr();

	s_devcon->VSSetShaderResources(srvSlot, 1, &srv);
	s_devcon->PSSetShaderResources(srvSlot, 1, &srv);
	s_devcon->HSSetShaderResources(srvSlot, 1, &srv);
	s_devcon->DSSetShaderResources(srvSlot, 1, &srv);
	s_devcon->GSSetShaderResources(srvSlot, 1, &srv);
}
