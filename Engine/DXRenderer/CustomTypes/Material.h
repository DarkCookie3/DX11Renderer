#pragma once
#include "../Managers/ShadersManager.h"
#include "../DXObjects/ShaderResourceView.h"
#include "../DXObjects/DepthStencilState.h"
#include "../DXObjects/RasterizerState.h"

using namespace DirectX;

struct ShadersGroup
{
	std::shared_ptr<VertexShader> vertexShader;
	std::shared_ptr<PixelShader> pixelShader;
	std::shared_ptr<HullShader> hullShader;
	std::shared_ptr<DomainShader> domainShader;
	std::shared_ptr<GeometryShader> geometryShader;
};

struct PipelineState
{
	DepthStencilState m_depthStencilState;
	RasterizerState m_rasterizerState;
	UINT stencilRefValue = 0;
};

struct Material
{
	std::shared_ptr<ShadersGroup> shaders;
	DxResPtr<ID3D11Texture2D> m_albedo;
	DxResPtr<ID3D11Texture2D> m_normal;
	DxResPtr<ID3D11Texture2D> m_roughness;
	DxResPtr<ID3D11Texture2D> m_metalness;
	DxResPtr<ID3D11Texture2D> m_emission;
	DxResPtr<ID3D11Texture2D> m_height;
	DxResPtr<ID3D11Texture2D> m_opacity;
	DxResPtr<ID3D11Texture2D> m_ambientOcclusion;
	DirectX::XMVECTOR albedoColor;
	DirectX::XMVECTOR emissionColor;
	std::shared_ptr<PipelineState> m_pipelineState;

	static Material CreateMat(std::shared_ptr<ShadersGroup> shaders_arg, const DirectX::XMVECTOR& albedoColor_arg)
	{
		Material mat{};
		mat.shaders = shaders_arg;
		mat.albedoColor = albedoColor_arg;
		return mat;
	}

	static Material CreateMat(std::shared_ptr<ShadersGroup> shaders_arg, const DirectX::XMVECTOR& albedoColor_arg,
							  const DirectX::XMVECTOR& emissionColor_arg, std::shared_ptr<PipelineState> pipelineState)
	{
		Material mat{};
		mat.m_pipelineState = pipelineState;
		mat.shaders = shaders_arg;
		mat.albedoColor = albedoColor_arg;
		mat.emissionColor = emissionColor_arg;
		return mat;
	}

	static Material CreateMat(std::shared_ptr<ShadersGroup> shaders_arg, DxResPtr<ID3D11Texture2D> albedo_arg,
							  const DirectX::XMVECTOR& albedoColor_arg, std::shared_ptr<PipelineState> pipelineState)
	{
		Material mat{};
		mat.shaders = shaders_arg;
		mat.m_albedo = albedo_arg;
		mat.albedoColor = albedoColor_arg;
		mat.m_pipelineState = pipelineState;
		return mat;
	}
};