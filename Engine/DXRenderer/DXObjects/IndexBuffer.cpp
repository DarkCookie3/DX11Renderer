#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const std::vector<uint32_t>& indices)
{
	if (indices.size() == 0)
	{
		return;
	}
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = indices.size() * sizeof(uint32_t);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	s_device->CreateBuffer(&indexBufferDesc, &indexData, m_buffer.reset());
}

IndexBuffer::~IndexBuffer()
{
}

void IndexBuffer::Bind()
{
	s_devcon->IASetIndexBuffer(m_buffer, DXGI_FORMAT_R32_UINT, 0);
}
