#pragma once
#include "../Utils/d3d.h"

struct VertexBuffer
{
public:
	VertexBuffer() = default;
	template <typename T>
	VertexBuffer(T* data, size_t size, bool immutable = true) : m_immutable(immutable)
	{
		if (size == 0)
		{
			return;
		}
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = size;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = immutable ? 0 : D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA subres_data;
		subres_data.pSysMem = data;
		subres_data.SysMemPitch = 0;
		subres_data.SysMemSlicePitch = 0;
		HRESULT H;
		if (data == nullptr)
		{
			H = s_device->CreateBuffer(&bd, nullptr, m_buffer.reset());
		}
		else
		{
			H = s_device->CreateBuffer(&bd, &subres_data, m_buffer.reset());
		} 
		ALWAYS_ASSERT(H == S_OK && "CreateVertexBuffer");
	}
	~VertexBuffer();

	void Bind(UINT slot, UINT stride, UINT offset);
	template <typename T>
	void Update(T* data, size_t size)
	{
		if (!m_immutable)
		{
			D3D11_MAPPED_SUBRESOURCE mapsubres;
			s_devcon->Map(m_buffer.ptr(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapsubres);
			memcpy(mapsubres.pData, data, size);
			s_devcon->Unmap(m_buffer.ptr(), NULL);
		}
		else
		{
			ALWAYS_ASSERT(FALSE && "Trying to update immutable buffer");
		}
	}
private:
	DxResPtr<ID3D11Buffer> m_buffer;
	bool m_immutable;
};