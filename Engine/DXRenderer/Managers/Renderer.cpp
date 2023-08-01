#include "Renderer.h"
#include "MeshSystem.h"
#include "LightSystem.h"
#include "PostProcess.h"
#include "../Utils/imgui/imgui.h"
#include "../Utils/imgui/imgui_impl_win32.h"
#include "../Utils/imgui/imgui_impl_dx11.h"
#include "ReflectionCapturer.h"
#include "TexturesManager.h"
#include "../../Common/Engine.h"
#include "ParticleSystem.h"
#include "../CustomTypes/GBuffer.h"


void Renderer::BindAllSamplersAndConstBuffers()
{
	int slot = 0;
	for (auto& buffer : m_constantBuffers)
	{
		if (buffer.active == true)
		{
			buffer.m_constantBuffer.Bind(slot);
		}
		++slot;
	}
	slot = 0;
	for (auto& sampler : m_samplerStates)
	{
		if (sampler.active == true)
		{
			sampler.m_samplerState.Bind(slot);
		}
		++slot;
	}
}

void Renderer::SetDebugShaders(std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader, std::shared_ptr<HullShader> hullShader, std::shared_ptr<DomainShader> domainShader, std::shared_ptr<GeometryShader> geometryShader)
{
	m_debugDraw.m_vertexShader = vertexShader;
	m_debugDraw.m_pixelShader = pixelShader;
	m_debugDraw.m_hullShader = hullShader;
	m_debugDraw.m_domainShader = domainShader;
	m_debugDraw.m_geometryShader = geometryShader;
}

void Renderer::SetNoiseMap(DxResPtr<ID3D11Texture2D> texture)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texture->GetDesc(&texDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	DxResPtr<ID3D11Resource> textureResource;
	texture->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(textureResource.reset()));

	m_noiseMap = ShaderResourceView(srvDesc, textureResource);
}

void Renderer::InitSkybox(std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader, const std::wstring& name)
{
	DxResPtr<ID3D11Texture2D> skyboxTexture;
	TexturesManager::GetInstance().GetTexture(skyboxTexture, name);
	m_skybox.Init(vertexShader, pixelShader, skyboxTexture);

	auto& reflCaptur = ReflectionCapturer::GetInstance();
	auto& irradianceMaps = reflCaptur.GetIrradianceTextures(Engine::GetInstance().GetWindow(), name);
	
	D3D11_TEXTURE2D_DESC texDesc;
	irradianceMaps.m_diffuse->GetDesc(&texDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;

	DxResPtr<ID3D11Resource> diffuseResource;
	irradianceMaps.m_diffuse->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(diffuseResource.reset()));

	m_diffuseIrradianceMap = ShaderResourceView(srvDesc, diffuseResource);
	
	irradianceMaps.m_specular->GetDesc(&texDesc);
	srvDesc.Format = texDesc.Format;
	srvDesc.TextureCube.MipLevels = texDesc.MipLevels;

	DxResPtr<ID3D11Resource> specularResource;
	irradianceMaps.m_specular->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(specularResource.reset()));

	m_specularIrradianceMap = ShaderResourceView(srvDesc, specularResource);

	irradianceMaps.m_BRDF->GetDesc(&texDesc);
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = texDesc.Format;
	srvDesc.TextureCube.MipLevels = texDesc.MipLevels;

	DxResPtr<ID3D11Resource> BRDFResource;
	irradianceMaps.m_BRDF->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(BRDFResource.reset()));

	m_specularBRDFPrecompute = ShaderResourceView(srvDesc, BRDFResource);
}

void Renderer::SetSkyboxCubemap(DxResPtr<ID3D11Texture2D> texture)
{
	m_skybox.SetSkyboxTexture(texture);
}

void Renderer::BindIrradianceMaps(uint32_t slotDiffuse, uint32_t slotSpecular, uint32_t slotBRDFSpecular)
{
	m_diffuseIrradianceMap.Bind(slotDiffuse);
	m_specularIrradianceMap.Bind(slotSpecular);
	m_specularBRDFPrecompute.Bind(slotBRDFSpecular);
}

void Renderer::BindDebugShader()
{
	s_devcon->VSSetShader(nullptr, nullptr, 0);
	s_devcon->HSSetShader(nullptr, nullptr, 0);
	s_devcon->DSSetShader(nullptr, nullptr, 0);
	s_devcon->GSSetShader(nullptr, nullptr, 0);
	s_devcon->PSSetShader(nullptr, nullptr, 0);

	if (m_debugDraw.m_vertexShader)
		m_debugDraw.m_vertexShader->Bind();

	if (m_debugDraw.m_pixelShader)
		m_debugDraw.m_pixelShader->Bind();

	if (m_debugDraw.m_hullShader)
		m_debugDraw.m_hullShader->Bind();

	if (m_debugDraw.m_domainShader)
		m_debugDraw.m_domainShader->Bind();

	if (m_debugDraw.m_geometryShader)
		m_debugDraw.m_geometryShader->Bind();
}

void Renderer::BindNoiseMap(uint32_t slot)
{
	m_noiseMap.Bind(slot);
}

void Renderer::Draw(WindowDX& window)
{
	window.m_backbufferRenderTargetLDR.Clear(m_clearColor);
	window.m_backbufferRenderTargetHDR.Bind();
	window.m_backbufferRenderTargetHDR.Clear(m_clearColor);

	auto ConstDataPerFrame = GetConstBufferData<CONSTANT_BUFFER_PER_FRAME_STRUCT>(BufferSlots::PER_FRAME);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		ImGui::Begin("Renderer state");
		ImGui::SetNextItemWidth(400);
		ImGui::SliderFloat("EV100", &ConstDataPerFrame->EV100, -20.0f, 20.0f);
		ImGui::Checkbox("EnableDiffuseIrradiance", (bool*)(&ConstDataPerFrame->enableDiffuseIrradiance));
		ImGui::Checkbox("EnableSpecularIrradiance", (bool*)(&ConstDataPerFrame->enableSpecularIrradiance));
		ImGui::Checkbox("OverwriteRoughness", (bool*)(&ConstDataPerFrame->overwriteRoughness));
		ImGui::Checkbox("OverwriteMetalness", (bool*)(&ConstDataPerFrame->overwriteMetalness));
		ImGui::SetNextItemWidth(400);
		ImGui::SliderFloat("RoughnessOverwriteValue", &ConstDataPerFrame->roughnessOverwriteValue, 0.02f, 0.98f);
		ImGui::SetNextItemWidth(400);
		ImGui::SliderFloat("MetalnessOverwriteValue", &ConstDataPerFrame->metalnessOverwriteValue, 0.02f, 0.98f);
		ImGui::End();
	}
	ImGui::Render();

	BindAllSamplersAndConstBuffers();
	
	BindIrradianceMaps(12, 13, 14);
	BindNoiseMap(18);

	auto& meshSystem = MeshSystem::GetInstance();
	auto& particleSystem = ParticleSystem::GetInstance();

	BindBlendState("opaqueBlend");
	meshSystem.Render(OpacityMeshGroup::Opaque, false, false, false);
	m_skybox.Render();

	BindBlendState("translucentOrderedBlend");
	meshSystem.Render(OpacityMeshGroup::Translucent, false, false, false);
	particleSystem.Render(window.m_backbufferRenderTargetHDR.GetDepthStencilBuffer());

	BindBlendState("opaqueBlend");
	auto& postProcess = PostProcess::GetInstance();
	postProcess.Resolve(window);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	window.Present();
}

void Renderer::DrawDeferred()
{
	static GBuffer gBuffer;
	auto& meshSystem = MeshSystem::GetInstance();
	auto& particleSystem = ParticleSystem::GetInstance();
	auto& shadersManager = ShadersManager::GetInstance();
	auto& window = Engine::GetInstance().GetWindow();

	if (window.resized) { gBuffer = GBuffer(); }

	auto ConstDataPerFrame = GetConstBufferData<CONSTANT_BUFFER_PER_FRAME_STRUCT>(BufferSlots::PER_FRAME);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		ImGui::Begin("Renderer state");
		ImGui::SetNextItemWidth(400);
		ImGui::SliderFloat("EV100", &ConstDataPerFrame->EV100, -20.0f, 20.0f);
		ImGui::Checkbox("EnableDiffuseIrradiance", (bool*)(&ConstDataPerFrame->enableDiffuseIrradiance));
		ImGui::Checkbox("EnableSpecularIrradiance", (bool*)(&ConstDataPerFrame->enableSpecularIrradiance));
		ImGui::Checkbox("OverwriteRoughness", (bool*)(&ConstDataPerFrame->overwriteRoughness));
		ImGui::Checkbox("OverwriteMetalness", (bool*)(&ConstDataPerFrame->overwriteMetalness));
		ImGui::SetNextItemWidth(400);
		ImGui::SliderFloat("RoughnessOverwriteValue", &ConstDataPerFrame->roughnessOverwriteValue, 0.02f, 0.98f);
		ImGui::SetNextItemWidth(400);
		ImGui::SliderFloat("MetalnessOverwriteValue", &ConstDataPerFrame->metalnessOverwriteValue, 0.02f, 0.98f);
		ImGui::End();
	}
	ImGui::Render();

	BindAllSamplersAndConstBuffers();
	BindNoiseMap(18);

	gBuffer.BindRTV();
	gBuffer.ClearRTV(m_clearColor);

	BindBlendState("opaqueBlend");
	//depth prepass
	s_devcon->HSSetShader(nullptr, nullptr, 0);
	s_devcon->DSSetShader(nullptr, nullptr, 0);
	s_devcon->GSSetShader(nullptr, nullptr, 0);
	shadersManager.GetVertexShader(L"FlatShadowVertex")->Bind();
	shadersManager.GetPixelShader(L"ShadowPixel")->Bind();
	meshSystem.Render(OpacityMeshGroup::Opaque, true, true, false);
	////////////////////////////
	meshSystem.Render(OpacityMeshGroup::Opaque, false, false, false);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = 0x00;
	depthStencilDesc.FrontFace = D3D11_DEPTH_STENCILOP_DESC{ D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
	depthStencilDesc.BackFace = D3D11_DEPTH_STENCILOP_DESC{ D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_NEVER };

	static PipelineState resolvePipelineState = { 
		DepthStencilState(depthStencilDesc),
		RasterizerState(RasterizerState::CreateRasterizerStateDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, 0, 0, false)),
		0 };

	window.m_backbufferRenderTargetLDR.Clear(m_clearColor);
	window.m_backbufferRenderTargetHDR.Clear(m_clearColor);
	window.m_backbufferRenderTargetHDR.Bind(gBuffer.GetDepthStencilView());

	gBuffer.BindSRV();
	BindIrradianceMaps(12, 13, 14);

	shadersManager.GetVertexShader(L"PostProcessVertex")->Bind();
	shadersManager.GetPixelShader(L"GBufferResolvePBR")->Bind();
	resolvePipelineState.m_rasterizerState.Bind();
	resolvePipelineState.m_depthStencilState.Bind(1);
	s_devcon->Draw(3, 0);

	shadersManager.GetPixelShader(L"GBufferResolveEmissive")->Bind();
	resolvePipelineState.m_depthStencilState.Bind(2);
	s_devcon->Draw(3, 0);

	m_skybox.Render();

	BindBlendState("translucentOrderedBlend");
	meshSystem.Render(OpacityMeshGroup::Translucent, false, false, false);
	particleSystem.Render(gBuffer.GetDepthStencilTexture());

	BindBlendState("opaqueBlend");
	auto& postProcess = PostProcess::GetInstance();
	postProcess.Resolve(window);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	window.Present();
}
