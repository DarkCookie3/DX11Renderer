#include "ShaderResourceView.h"

ShaderResourceView::ShaderResourceView(DxResPtr<ID3D11Resource> pResource)
{
	HRESULT H = s_device->CreateShaderResourceView(pResource.ptr(), nullptr, m_srv.reset());
	ALWAYS_ASSERT(H == S_OK && "CreateShaderResourceView");
}

ShaderResourceView::ShaderResourceView(D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc, DxResPtr<ID3D11Resource> pResource)
{
	HRESULT H = s_device->CreateShaderResourceView(pResource.ptr(), &srvDesc, m_srv.reset());
	ALWAYS_ASSERT(H == S_OK && "CreateShaderResourceView");
}

void ShaderResourceView::Bind(uint32_t slot)
{
	auto* const srv = m_srv.ptr();
	if (!srv) return;

	s_devcon->VSSetShaderResources(slot, 1, &srv);
	s_devcon->HSSetShaderResources(slot, 1, &srv);
	s_devcon->DSSetShaderResources(slot, 1, &srv);
	s_devcon->GSSetShaderResources(slot, 1, &srv);
	s_devcon->PSSetShaderResources(slot, 1, &srv);
}

void ShaderResourceView::GenerateMips()
{
	s_devcon->GenerateMips(m_srv);
}
