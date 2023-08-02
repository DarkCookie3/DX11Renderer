#include "GBuffer.h"
#include "../../Common/Engine.h"

GBuffer::GBuffer()
{
	std::array<D3D11_TEXTURE2D_DESC, 4> texDesc;

	ZeroMemory(texDesc.data(), sizeof(D3D11_TEXTURE2D_DESC) * 4);

	auto& window = Engine::GetInstance().GetWindow();

	texDesc[0].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc[1].Format = DXGI_FORMAT_R8G8_UNORM;
	texDesc[2].Format = DXGI_FORMAT_R16G16B16A16_SNORM;
	texDesc[3].Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

	for (int i = 0; i < texDesc.size(); i++)
	{
		texDesc[i].ArraySize = 1;
		texDesc[i].BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texDesc[i].Height = window.m_windowH;
		texDesc[i].Width = window.m_windowW;
		texDesc[i].MipLevels = 1;
		texDesc[i].SampleDesc.Count = 1;
		texDesc[i].Usage = D3D11_USAGE_DEFAULT;

		s_device->CreateTexture2D(&texDesc[i], nullptr, m_textures[i].reset());
	}

	for (int i = 0; i < m_srv.size(); i++)
	{
		DxResPtr<ID3D11Resource> resource;
		m_textures[i]->QueryInterface(resource.reset());
		m_srv[i] = ShaderResourceView(resource);
	}

	for (int i = 0; i < m_rtv.size(); i++)
	{
		s_device->CreateRenderTargetView(m_textures[i], nullptr, m_rtv[i].reset());
	}

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width = window.m_windowW;
	depthStencilDesc.Height = window.m_windowH;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	s_device->CreateTexture2D(&depthStencilDesc, NULL, m_depthStencilTexture.reset());
	s_device->CreateTexture2D(&depthStencilDesc, NULL, m_depthStencilTextureCopy.reset());

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	s_device->CreateDepthStencilView(m_depthStencilTexture, &descDSV, m_depthStencilView.reset());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDepthStencilDesc;
	srvDepthStencilDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDepthStencilDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDepthStencilDesc.Texture2D.MipLevels = 1;
	srvDepthStencilDesc.Texture2D.MostDetailedMip = 0;

	DxResPtr<ID3D11Resource> depthStencilCopyResource;
	m_depthStencilTextureCopy->QueryInterface(depthStencilCopyResource.reset());
	m_depthStencilTextureCopySRV = ShaderResourceView(srvDepthStencilDesc, depthStencilCopyResource);
}

void GBuffer::BindSRV()
{
	for (int i = 0; i < m_srv.size(); i++)
	{
		m_srv[i].Bind(i);
	}
	s_devcon->CopyResource(m_depthStencilTextureCopy, m_depthStencilTexture);
	m_depthStencilTextureCopySRV.Bind(5);
}

void GBuffer::BindRTV()
{
	s_devcon->OMSetRenderTargets(m_rtv.size(), (ID3D11RenderTargetView**)m_rtv.data(), m_depthStencilView);
}

void GBuffer::BindDepthStencilCopy(UINT slot)
{
	m_depthStencilTextureCopySRV.Bind(slot);
}

void GBuffer::ClearRTV(FLOAT* albedoColor)
{
	static FLOAT zeroColor[4] = { 0, 0, 0, 0 };
	s_devcon->ClearRenderTargetView(m_rtv[0], albedoColor);
	for (int i = 1; i < m_rtv.size(); i++)
	{
		s_devcon->ClearRenderTargetView(m_rtv[i], zeroColor);
	}
	s_devcon->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
}
