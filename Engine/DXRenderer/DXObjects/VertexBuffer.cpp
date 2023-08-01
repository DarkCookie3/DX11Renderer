#include "VertexBuffer.h"

VertexBuffer::~VertexBuffer()
{
}

void VertexBuffer::Bind(UINT slot, UINT stride, UINT offset)
{
	auto const bufferPtr = m_buffer.ptr();
	s_devcon->IASetVertexBuffers(slot, 1, &bufferPtr, &stride, &offset);
}
