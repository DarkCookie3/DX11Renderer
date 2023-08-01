#include "Skybox.h"

void Skybox::Init(std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader, const DxResPtr<ID3D11Texture2D>& texture)
{
	m_vertexShader = vertexShader;
	m_pixelShader = pixelShader;

	auto depthStencilState = DepthStencilState(DepthStencilState::CreateDepthStencilStateDesc(true, true, D3D11_DEPTH_WRITE_MASK_ZERO));
	auto rasterizerState = RasterizerState(RasterizerState::CreateRasterizerStateDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, 0, 0, false));

	m_pipelineState = PipelineState{ depthStencilState, rasterizerState, 2 };

	SetSkyboxTexture(texture);
}

void Skybox::Render()
{
	if (!m_skycubeTexture.ptr())
	{
		return;
	}

	s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	s_devcon->HSSetShader(nullptr, nullptr, 0);
	s_devcon->DSSetShader(nullptr, nullptr, 0);
	s_devcon->GSSetShader(nullptr, nullptr, 0);

	m_vertexShader->Bind();
	m_pixelShader->Bind();

	BindSRV(0);
	
	m_pipelineState.m_depthStencilState.Bind(m_pipelineState.stencilRefValue);
	m_pipelineState.m_rasterizerState.Bind();

	s_devcon->Draw(3, 0);
}

void Skybox::SetSkyboxTexture(const DxResPtr<ID3D11Texture2D>& texture)
{
	m_skycubeTexture = texture;

	D3D11_TEXTURE2D_DESC texDesc;
	m_skycubeTexture->GetDesc(&texDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;

	DxResPtr<ID3D11Resource> resource;
	m_skycubeTexture->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(resource.reset()));

	m_shaderResourceView = ShaderResourceView(srvDesc, resource);
}
