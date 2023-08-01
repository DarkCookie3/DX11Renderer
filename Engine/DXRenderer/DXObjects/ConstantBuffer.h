#pragma once
#include "../Utils/d3d.h"

struct CONSTANT_BUFFER_PER_TEXTURE_STRUCT
{
	int inputWidth;
	int inputHeight;
	float texelSize;
	float roughness;
};

struct CONSTANT_BUFFER_ADDITIONAL_PER_VIEW_STRUCT
{
	DirectX::XMMATRIX viewProjCubeSides[6];
};

struct CONSTANT_BUFFER_PER_VIEW_STRUCT
{
	DirectX::XMMATRIX viewProj;
	DirectX::XMMATRIX viewProjInv;
	DirectX::XMMATRIX viewInv;
	DirectX::XMMATRIX projInv;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
	DirectX::XMVECTOR camera_pos;
};

struct CONSTANT_BUFFER_PER_FRAME_STRUCT
{
	float g_time;
	float EV100;
	int pointLightNum;
	int spotLightNum;
	int dirLightNum;
	BOOL enableDiffuseIrradiance;
	BOOL enableSpecularIrradiance;
	BOOL overwriteRoughness;
	BOOL overwriteMetalness;
	float roughnessOverwriteValue;
	float metalnessOverwriteValue;
	int padding;
};

class ConstantBuffer
{
public:
	ConstantBuffer() = default;
	~ConstantBuffer() = default;

	template <typename T>
	ConstantBuffer(T* data, size_t size)
	{
		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = size;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = data;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		HRESULT H = s_device->CreateBuffer(&cbDesc, &InitData, m_buffer.reset());
		ALWAYS_ASSERT(H == S_OK && "Create ConstantBuffer");
	}

	void Bind(UINT slot)
	{
		auto const constBuf = m_buffer.ptr();
		s_devcon->VSSetConstantBuffers(slot, 1, &constBuf);
		s_devcon->PSSetConstantBuffers(slot, 1, &constBuf);
		s_devcon->HSSetConstantBuffers(slot, 1, &constBuf);
		s_devcon->DSSetConstantBuffers(slot, 1, &constBuf);
		s_devcon->GSSetConstantBuffers(slot, 1, &constBuf);
	}

	template <typename T>
	void Update(T* data, size_t size)
	{
		D3D11_MAPPED_SUBRESOURCE mapsubres;
		HRESULT H = s_devcon->Map(m_buffer.ptr(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapsubres);
		memcpy(mapsubres.pData, data, size);
		s_devcon->Unmap(m_buffer.ptr(), NULL);
	}

private:
	DxResPtr<ID3D11Buffer> m_buffer;
};