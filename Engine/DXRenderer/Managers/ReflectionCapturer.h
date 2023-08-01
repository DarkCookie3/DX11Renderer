#pragma once
#include "../Utils/d3d.h"
#include <unordered_map>
#include "../Window/WindowDX.h"
#include "../DXObjects/VertexShader.h"
#include "../DXObjects/PixelShader.h"

struct IBLTextures
{
	DxResPtr<ID3D11Texture2D> m_diffuse;
	DxResPtr<ID3D11Texture2D> m_specular;
	DxResPtr<ID3D11Texture2D> m_BRDF;
};

class ReflectionCapturer
{
public:

	static ReflectionCapturer& GetInstance()
	{
		static ReflectionCapturer instance;
		return instance;
	}
	ReflectionCapturer(const ReflectionCapturer&) = delete;
	ReflectionCapturer& operator=(const ReflectionCapturer&) = delete;

	void SetIrradianceTexturesCreationShaders(std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> diffuseIrradianceShader, std::shared_ptr<PixelShader> specularIrradianceShader, std::shared_ptr<PixelShader> specularBRDFPrecomputeShader);

	IBLTextures& GetIrradianceTextures(WindowDX& window, const std::wstring& name);
private:
	std::unordered_map<std::wstring, IBLTextures> m_irradianceTextures = {};
	std::shared_ptr<VertexShader> m_vertexShader;
	std::shared_ptr<PixelShader> m_diffuseIrradianceShader;
	std::shared_ptr<PixelShader> m_specularIrradianceShader;
	std::shared_ptr<PixelShader> m_specularBRDFPrecomputeShader;

	bool LoadIBLTextures(IBLTextures& outTextures, const std::wstring& name);
	IBLTextures GenerateIBLTextures(WindowDX& window, const std::wstring& name);

	ReflectionCapturer() {}
};