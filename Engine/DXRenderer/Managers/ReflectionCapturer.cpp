#include "ReflectionCapturer.h"
#include "TexturesManager.h"
#include "Renderer.h"
#include <algorithm>
#include <filesystem>
#include "../Utils/CubeSideIndexRoationAngles.h"

void RenderCubeSide(D3D11_TEXTURE2D_DESC TexDesc, int side, int mip, DxResPtr<ID3D11Texture2D> result, WindowDX& window, float roughness = 0.0f)
{
	auto& renderer = Renderer::GetInstance();

	auto ConstDataPerView = renderer.GetConstBufferData<CONSTANT_BUFFER_PER_VIEW_STRUCT>(BufferSlots::PER_VIEW);
	auto ConstDataIblGen = renderer.GetConstBufferData<CONSTANT_BUFFER_PER_TEXTURE_STRUCT>(BufferSlots::IBL_GEN);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = TexDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.MipSlice = mip;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.FirstArraySlice = side;

	DxResPtr<ID3D11RenderTargetView> RTV;
	s_device->CreateRenderTargetView(result, &rtvDesc, RTV.reset());
	const auto diffuseIrradianceRTVconst = RTV.ptr();
	s_devcon->OMSetRenderTargets(1, &diffuseIrradianceRTVconst, 0);

	CameraDX camera = CameraDX({ 0, 0, 0, 1 }, { 0, 0, 1, 1 }, 0.1f, 10.0f, 90.0f);
	float yawRotation;
	float pitchRotation;
	GetCubeSideCaptureAngles(yawRotation, pitchRotation, side);
	camera.Rotate(0, pitchRotation, yawRotation);

	auto view = camera.generateViewMat();
	auto proj = camera.generateProjMatReversedZ(float(window.m_windowW) / window.m_windowH);
	ConstDataPerView->viewProj = DirectX::XMMatrixMultiply(view, proj);
	ConstDataPerView->viewInv = camera.m_transform.ToMat4();
	ConstDataPerView->projInv = XMMatrixInverse(0, proj);
	ConstDataPerView->view = view;
	ConstDataPerView->proj = proj;
	ConstDataPerView->camera_pos = camera.m_transform.position;
	renderer.UpdateConstBufferAtSlot<CONSTANT_BUFFER_PER_VIEW_STRUCT>(BufferSlots::PER_VIEW);
	ConstDataIblGen->roughness = roughness;
	ConstDataIblGen->inputWidth = TexDesc.Width;
	ConstDataIblGen->inputHeight = TexDesc.Height;
	renderer.UpdateConstBufferAtSlot<CONSTANT_BUFFER_PER_TEXTURE_STRUCT>(BufferSlots::IBL_GEN);
	renderer.BindAllSamplersAndConstBuffers();
	s_devcon->Draw(3, 0);
}

void ReflectionCapturer::SetIrradianceTexturesCreationShaders(std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> diffuseIrradianceShader, std::shared_ptr<PixelShader> specularIrradianceShader, std::shared_ptr<PixelShader> specularBRDFPrecomputeShader)
{
	m_specularBRDFPrecomputeShader = specularBRDFPrecomputeShader;
	m_diffuseIrradianceShader = diffuseIrradianceShader;
	m_specularIrradianceShader = specularIrradianceShader;
	m_vertexShader = vertexShader;
}

IBLTextures& ReflectionCapturer::GetIrradianceTextures(WindowDX& window, const std::wstring& name)
{
	auto it = m_irradianceTextures.find(name);
	if (it != m_irradianceTextures.end())
	{
		return it->second;
	}
	else
	{
		IBLTextures result;
		if (!LoadIBLTextures(result, name))
		{
			result = GenerateIBLTextures(window, name);
		}
		auto emplaced = m_irradianceTextures.emplace(name, result);
		return emplaced.first->second;
	}
}

bool ReflectionCapturer::LoadIBLTextures(IBLTextures& outTextures, const std::wstring& name)
{
	auto filepath = std::filesystem::path(name);
	auto& texturesManager = TexturesManager::GetInstance();
	if (!texturesManager.GetTexture(outTextures.m_BRDF, filepath.filename().stem().wstring() + L"_IBL_BRDF" + filepath.extension().wstring()) ||
		!texturesManager.GetTexture(outTextures.m_diffuse, filepath.filename().stem().wstring() + L"_IBL_DIFFUSE" + filepath.extension().wstring()) ||
		!texturesManager.GetTexture(outTextures.m_specular, filepath.filename().stem().wstring() + L"_IBL_SPECULAR" + filepath.extension().wstring()))
	{
		return false;
	}
	return true;
}

IBLTextures ReflectionCapturer::GenerateIBLTextures(WindowDX& window, const std::wstring& name)
{
	IBLTextures result;
	auto initWidth = window.m_windowW;
	auto initHeight = window.m_windowH;

	auto& texturesManager = TexturesManager::GetInstance();
	DxResPtr<ID3D11Texture2D> skybox;
	texturesManager.GetTexture(skybox, name);
	D3D11_TEXTURE2D_DESC texDesc;
	skybox->GetDesc(&texDesc);
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;
	DxResPtr<ID3D11Resource> skyboxResource;
	skybox->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(skyboxResource.reset()));
	auto skyboxSRV = ShaderResourceView(srvDesc, skyboxResource);

	skyboxSRV.Bind(0);

	D3D11_TEXTURE2D_DESC skyboxDesc;
	skybox->GetDesc(&skyboxDesc);

	s_devcon->HSSetShader(nullptr, nullptr, 0);
	s_devcon->DSSetShader(nullptr, nullptr, 0);
	s_devcon->GSSetShader(nullptr, nullptr, 0);
	m_vertexShader->Bind();

	D3D11_TEXTURE2D_DESC diffuseIrradianceTexDesc = skyboxDesc;
	diffuseIrradianceTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	diffuseIrradianceTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	diffuseIrradianceTexDesc.MipLevels = 1;
	diffuseIrradianceTexDesc.Width = 32;
	diffuseIrradianceTexDesc.Height = 32;

	window.ResizeViewport(diffuseIrradianceTexDesc.Width, diffuseIrradianceTexDesc.Height);

	HRESULT H = s_device->CreateTexture2D(&diffuseIrradianceTexDesc, nullptr, result.m_diffuse.reset());

	m_diffuseIrradianceShader->Bind();

	for (int i = 0; i < 6; i++)
	{
		RenderCubeSide(diffuseIrradianceTexDesc, i, 0, result.m_diffuse, window);
	}
	
	m_specularIrradianceShader->Bind();

	int maxMipSpecIrrad = 1 + floor(log2(std::max(skyboxDesc.Width, skyboxDesc.Height)));
	int mipWidth = skyboxDesc.Width;
	int mipHeight = skyboxDesc.Height;

	D3D11_TEXTURE2D_DESC specularIrradianceTexDesc = skyboxDesc;
	specularIrradianceTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	specularIrradianceTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	specularIrradianceTexDesc.MipLevels = maxMipSpecIrrad;
	specularIrradianceTexDesc.Width = mipWidth;
	specularIrradianceTexDesc.Height = mipHeight;

	H = s_device->CreateTexture2D(&specularIrradianceTexDesc, nullptr, result.m_specular.reset());

	for (int k = 0; k < maxMipSpecIrrad; k++)
	{
		window.ResizeViewport(mipWidth, mipHeight);

		float roughnessForMip = float(k) / (maxMipSpecIrrad - 1.0f);

		for (int i = 0; i < 6; i++)
		{
			RenderCubeSide(specularIrradianceTexDesc, i, k, result.m_specular, window, roughnessForMip);
		}
		mipWidth = mipWidth == 1 ? 1 : mipWidth / 2;
		mipHeight = mipHeight == 1 ? 1 : mipHeight / 2;
	}
	
	window.ResizeViewport(256, 256);

	m_specularBRDFPrecomputeShader->Bind();

	D3D11_TEXTURE2D_DESC brdfTexDesc;
	brdfTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	brdfTexDesc.Format = DXGI_FORMAT_R16G16_FLOAT;
	brdfTexDesc.Usage = D3D11_USAGE_DEFAULT;
	brdfTexDesc.SampleDesc.Count = 1;
	brdfTexDesc.SampleDesc.Quality = 0;
	brdfTexDesc.ArraySize = 1;
	brdfTexDesc.MipLevels = 1;
	brdfTexDesc.Width = 256;
	brdfTexDesc.Height = 256;
	brdfTexDesc.CPUAccessFlags = 0;
	brdfTexDesc.MiscFlags = 0;

	H = s_device->CreateTexture2D(&brdfTexDesc, nullptr, result.m_BRDF.reset());

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = brdfTexDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	DxResPtr<ID3D11RenderTargetView> RTVbrdf;
	s_device->CreateRenderTargetView(result.m_BRDF, &rtvDesc, RTVbrdf.reset());
	const auto brdfRTVconst = RTVbrdf.ptr();
	s_devcon->OMSetRenderTargets(1, &brdfRTVconst, 0);

	auto& renderer = Renderer::GetInstance();
	auto ConstDataIblGen = renderer.GetConstBufferData<CONSTANT_BUFFER_PER_TEXTURE_STRUCT>(BufferSlots::IBL_GEN);
	ConstDataIblGen->inputWidth = brdfTexDesc.Width;
	ConstDataIblGen->inputHeight = brdfTexDesc.Height;
	renderer.UpdateConstBufferAtSlot<CONSTANT_BUFFER_PER_TEXTURE_STRUCT>(BufferSlots::IBL_GEN);
	renderer.BindAllSamplersAndConstBuffers();
	s_devcon->Draw(3, 0);

	window.ResizeViewport(initWidth, initHeight);

	auto filepath = std::filesystem::path(name);
	texturesManager.SaveTexture(result.m_BRDF, filepath.filename().stem().wstring() + L"_IBL_BRDF" + filepath.extension().wstring());
	texturesManager.SaveTexture(result.m_diffuse, filepath.filename().stem().wstring() + L"_IBL_DIFFUSE" + filepath.extension().wstring());
	texturesManager.SaveTexture(result.m_specular, filepath.filename().stem().wstring() + L"_IBL_SPECULAR" + filepath.extension().wstring());

	return result;
}
