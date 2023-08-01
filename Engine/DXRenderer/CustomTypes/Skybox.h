#pragma once
#include "../Utils/d3d.h"
#include "../DXObjects/VertexShader.h"
#include "../DXObjects/PixelShader.h"
#include "../DXObjects/ShaderResourceView.h"
#include "Material.h"

class Skybox
{
public:
	void Init(std::shared_ptr<VertexShader> m_vertexShader, std::shared_ptr<PixelShader> m_pixelShader, const DxResPtr<ID3D11Texture2D>& texture);
	void Render();
	void SetSkyboxTexture(const DxResPtr<ID3D11Texture2D>& texture);
private:
	void BindSRV(uint32_t slot) { m_shaderResourceView.Bind(0); }

	ShaderResourceView m_shaderResourceView;
	std::shared_ptr<VertexShader> m_vertexShader;
	std::shared_ptr<PixelShader> m_pixelShader;
	DxResPtr<ID3D11Texture2D> m_skycubeTexture;
	PipelineState m_pipelineState;
};