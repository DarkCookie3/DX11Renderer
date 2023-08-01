#include "PostProcess.h"
#include "../DXObjects/ShaderResourceView.h"

void PostProcess::SetPostProcessShaders(std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader)
{
	m_vertexShader = vertexShader;
	m_pixelShader = pixelShader;

	auto depthStencilState = DepthStencilState(DepthStencilState::CreateDepthStencilStateDesc(false, false));
	auto rasterizerState = RasterizerState(RasterizerState::CreateRasterizerStateDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, 0, 0, false));

	m_pipelineState = PipelineState{ depthStencilState, rasterizerState, 0 };
}

void PostProcess::Resolve(WindowDX& window)
{
	s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pipelineState.m_depthStencilState.Bind(m_pipelineState.stencilRefValue);
	m_pipelineState.m_rasterizerState.Bind();
	window.m_backbufferRenderTargetLDR.Bind();

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	DxResPtr<ID3D11Resource> backbufferResource;
	window.m_backbufferRenderTargetHDR.GetBackBuffer()->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(backbufferResource.reset()));
	
	auto backbufferSRV = ShaderResourceView(srvDesc, backbufferResource);
	backbufferSRV.Bind(0);

	m_vertexShader->Bind();
	m_pixelShader->Bind();

	s_devcon->Draw(3, 0);

	ID3D11ShaderResourceView* pSRV = nullptr;
	s_devcon->VSSetShaderResources(0, 1, &pSRV);
	s_devcon->HSSetShaderResources(0, 1, &pSRV);
	s_devcon->DSSetShaderResources(0, 1, &pSRV);
	s_devcon->GSSetShaderResources(0, 1, &pSRV);
	s_devcon->PSSetShaderResources(0, 1, &pSRV);
}
