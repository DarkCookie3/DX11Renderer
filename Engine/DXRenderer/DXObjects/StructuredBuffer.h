#pragma once
#include "ShaderResourceView.h"

class StructuredBuffer
{
public:
	StructuredBuffer() = default;
	template <typename T>
	StructuredBuffer(T* data, UINT sizeOfOneElem, UINT numElements) : m_stride{ sizeOfOneElem * numElements }
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = m_stride;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeOfOneElem;

		D3D11_SUBRESOURCE_DATA subres_data;
		subres_data.pSysMem = data;
		subres_data.SysMemPitch = 0;
		subres_data.SysMemSlicePitch = 0;

		HRESULT hr;
		if (data == nullptr)
		{
			hr = s_device->CreateBuffer(&desc, nullptr, m_buffer.reset());
		}
		else
		{
			hr = s_device->CreateBuffer(&desc, &subres_data, m_buffer.reset());
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.ElementOffset = 0;
		srvDesc.Buffer.ElementWidth = sizeOfOneElem;
		srvDesc.Buffer.NumElements = numElements;
		hr = s_device->CreateShaderResourceView(m_buffer, &srvDesc, m_srv.reset());
	}
	~StructuredBuffer();

	template <typename T>
	void Update(T* data)
	{
		s_devcon->UpdateSubresource(m_buffer, 0, 0, data, 0, 0);
	}
	void Bind(uint32_t srvSlot);
private:
	UINT m_stride;
	DxResPtr<ID3D11Buffer> m_buffer;
	DxResPtr<ID3D11ShaderResourceView> m_srv;
};