#pragma once
#include <array>
#include "../Utils/d3d.h"
#include "../DXObjects/ShaderResourceView.h"

struct GBuffer
{
	GBuffer();
	void BindSRV();
	void BindRTV();
	void BindDepthStencilCopy(UINT slot);
	void ClearRTV(FLOAT* albedoColor);

	DxResPtr<ID3D11DepthStencilView> GetDepthStencilView() { return m_depthStencilView; }
	DxResPtr<ID3D11Texture2D> GetDepthStencilTexture() { return m_depthStencilTexture; }

private:
	DxResPtr<ID3D11DepthStencilView> m_depthStencilView;
	DxResPtr<ID3D11Texture2D> m_depthStencilTexture;

	DxResPtr<ID3D11Texture2D> m_depthStencilTextureCopy;
	ShaderResourceView m_depthStencilTextureCopySRV;

	std::array<DxResPtr<ID3D11Texture2D>, 5> m_textures;
	std::array<ShaderResourceView, 5> m_srv;
	std::array<DxResPtr<ID3D11RenderTargetView>, 5> m_rtv;
};