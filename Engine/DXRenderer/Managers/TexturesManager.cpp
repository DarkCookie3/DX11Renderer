#include "TexturesManager.h"

using namespace DirectX;

size_t GetDXGIFormatSize(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R32G32B32A32_FLOAT: return 16;
	case DXGI_FORMAT_R16G16B16A16_FLOAT: return 8;
	case DXGI_FORMAT_R8G8B8A8_UNORM: return 4;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return 4;
	case DXGI_FORMAT_R32G32_FLOAT: return 8;
	case DXGI_FORMAT_R16G16_FLOAT: return 4;
	case DXGI_FORMAT_R32_FLOAT: return 4;
	case DXGI_FORMAT_R16_FLOAT: return 2;
	case DXGI_FORMAT_R8_UNORM: return 1;
	default: return 0;
	}
}



bool TexturesManager::AddCubemapTexture(DxResPtr<ID3D11Texture2D>& cubemap, const std::wstring& name)
{
	std::wstring path = TEXTURES_DIR + name;
	DxResPtr<ID3D11Resource> resource;
	HRESULT hr = CreateDDSTextureFromFileEx(
		s_device,
		s_devcon,
		path.c_str(),
		0,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE,    
		0,
		D3D11_RESOURCE_MISC_TEXTURECUBE,
		DDS_LOADER_DEFAULT,
		resource.reset(),
		nullptr
		);

	if (hr != S_OK)
	{
		return false;
	}

	DxResPtr<ID3D11Texture2D> result;
	resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)result.reset());

	auto inserted = m_textures.emplace(name, result);
	cubemap = inserted.first->second;
	return true;
}

DxResPtr<ID3D11Texture2D>& TexturesManager::AddTextureOfModel(const std::wstring& filepath)
{
	DxResPtr<ID3D11Resource> resource;
	HRESULT hr = CreateDDSTextureFromFile(s_device, filepath.c_str(), resource.reset(), nullptr);
	ALWAYS_ASSERT(hr == S_OK && "CreateTextureFromFile");

	DxResPtr<ID3D11Texture2D> result;
	resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)result.reset());

	std::wstring separator = L"/\\"; // use '\\' for Windows paths

	size_t pos = filepath.find_last_of(separator);

	std::wstring dirname = filepath.substr(0, pos);
	size_t pos2 = dirname.find_last_of(separator);
		
	dirname = dirname.substr(0, pos2);
	size_t pos3 = dirname.find_last_of(separator);

	std::wstring name = filepath.substr(pos3 + 1, pos2 - pos3) + filepath.substr(pos + 1, filepath.size() - 1);

	auto inserted = m_textures.emplace(name, result);
	return inserted.first->second;
}

bool TexturesManager::GetTexture(DxResPtr<ID3D11Texture2D>& texture, const std::wstring& name)
{
	auto it = m_textures.find(name);
	if (it != m_textures.end())
	{
		texture = it->second;
		return true;
	}
	else
	{
		return (LoadTexture(texture, name));
	}
}

void TexturesManager::SaveTexture(DxResPtr<ID3D11Texture2D>& texture, const std::wstring& name)
{
	std::wstring path = TEXTURES_DIR + name;

	D3D11_TEXTURE2D_DESC texDesc;
	texture->GetDesc(&texDesc);

	size_t bytesPerPixel = GetDXGIFormatSize(texDesc.Format);

	D3D11_TEXTURE2D_DESC stagingTexDesc = texDesc;
	stagingTexDesc.Usage = D3D11_USAGE_STAGING;
	stagingTexDesc.BindFlags = 0;
	stagingTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	DxResPtr<ID3D11Texture2D> stagingTexture;
	HRESULT H = s_device->CreateTexture2D(&stagingTexDesc, nullptr, stagingTexture.reset());
	s_devcon->CopyResource(stagingTexture, texture);

	DirectX::ScratchImage scratchImage;
	if (texDesc.ArraySize == 1)
	{
		scratchImage.Initialize2D(texDesc.Format, texDesc.Width, texDesc.Height, 1, texDesc.MipLevels);
	}
	else
	{
		scratchImage.InitializeCube(texDesc.Format, texDesc.Width, texDesc.Height, 1, texDesc.MipLevels);
	}

	for (UINT arraySlice = 0; arraySlice < texDesc.ArraySize; ++arraySlice)
	{
		for (UINT mip = 0; mip < texDesc.MipLevels; ++mip)
		{
			D3D11_MAPPED_SUBRESOURCE mapsubres;
			H = s_devcon->Map(stagingTexture, D3D11CalcSubresource(mip, arraySlice, texDesc.MipLevels), D3D11_MAP_READ, 0, &mapsubres);

			const DirectX::Image* img = scratchImage.GetImage(mip, arraySlice, 0);

			for (size_t h = 0; h < img->height; ++h)
			{
				memcpy(
					img->pixels + h * img->rowPitch,
					(uint8_t*)mapsubres.pData + h * mapsubres.RowPitch,
					img->width * bytesPerPixel
				);
			}

			s_devcon->Unmap(stagingTexture, D3D11CalcSubresource(mip, arraySlice, texDesc.MipLevels));
		}
	}


	H = DirectX::SaveToDDSFile(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, path.c_str());
}

bool TexturesManager::LoadTexture(DxResPtr<ID3D11Texture2D>& texture, const std::wstring& name)
{
	std::wstring path = TEXTURES_DIR + name;

	DxResPtr<ID3D11Resource> resource;
	HRESULT hr = CreateDDSTextureFromFile(s_device, path.c_str(), resource.reset(), nullptr);
	if (hr != S_OK)
	{
		return false;
	}

	DxResPtr<ID3D11Texture2D> result;
	resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)result.reset());

	auto inserted = m_textures.emplace(name, result);
	texture = inserted.first->second;
	return true;
}
