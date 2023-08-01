#pragma once
#include "../Utils/d3d.h"

class ShaderResourceView
{
public:
	ShaderResourceView() = default;
	ShaderResourceView(DxResPtr<ID3D11Resource> pResource);
	ShaderResourceView(D3D11_SHADER_RESOURCE_VIEW_DESC desc, DxResPtr<ID3D11Resource> pResource);

	void Bind(uint32_t slot);
	void GenerateMips();
private:
	DxResPtr<ID3D11ShaderResourceView> m_srv;
};