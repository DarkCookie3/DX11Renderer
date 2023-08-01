#pragma once
#include <unordered_map>
#include "../Utils/d3d.h"

#define TEXTURES_DIR L"../../Engine/DXRenderer/Resources/Textures/"

class TexturesManager
{
public:

	static TexturesManager& GetInstance()
	{
		static TexturesManager instance;
		return instance;
	}
	TexturesManager(const TexturesManager&) = delete;
	TexturesManager& operator=(const TexturesManager&) = delete;

	bool AddCubemapTexture(DxResPtr<ID3D11Texture2D>& cubemap, const std::wstring& name);
	DxResPtr<ID3D11Texture2D>& AddTextureOfModel(const std::wstring& filepath);
	void SaveTexture(DxResPtr<ID3D11Texture2D>& texture, const std::wstring& name);
	bool GetTexture(DxResPtr<ID3D11Texture2D>& texture, const std::wstring& name);

private:
	std::unordered_map<std::wstring, DxResPtr<ID3D11Texture2D>> m_textures = {};
	bool LoadTexture(DxResPtr<ID3D11Texture2D>& texture, const std::wstring& name);

	TexturesManager() {}
};