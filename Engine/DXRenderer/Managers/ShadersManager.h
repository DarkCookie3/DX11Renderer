#pragma once
#include <unordered_map>
#include "../DXObjects/VertexShader.h"
#include "../DXObjects/PixelShader.h"
#include "../DXObjects/HullShader.h"
#include "../DXObjects/GeometryShader.h"
#include "../DXObjects/DomainShader.h"

class ShadersManager
{
public:
	std::shared_ptr<VertexShader>& AddVertexShader(const LPCWSTR& filepath, std::vector<D3D11_INPUT_ELEMENT_DESC> desc)
	{
		VertexShader shader(filepath, desc);
		auto inserted = m_vertexShaders.insert({ shader.GetFileName(), std::make_shared<VertexShader>(shader)});
		return inserted.first->second;
	}
	std::shared_ptr<PixelShader>& AddPixelShader(const LPCWSTR& filepath)
	{
		PixelShader shader(filepath);
		auto inserted = m_pixelShaders.insert({ shader.GetFileName(), std::make_shared<PixelShader>(shader) });
		return inserted.first->second;
	}
	std::shared_ptr<HullShader>& AddHullShader(const LPCWSTR& filepath)
	{
		HullShader shader(filepath);
		auto inserted = m_hullShaders.insert({ shader.GetFileName(), std::make_shared<HullShader>(shader) });
		return inserted.first->second;
	}
	std::shared_ptr<DomainShader>& AddDomainShader(const LPCWSTR& filepath)
	{
		DomainShader shader(filepath);
		auto inserted = m_domainShaders.insert({ shader.GetFileName(), std::make_shared<DomainShader>(shader) });
		return inserted.first->second;
	}
	std::shared_ptr<GeometryShader>& AddGeometryShader(const LPCWSTR& filepath)
	{
		GeometryShader shader(filepath);
		auto inserted = m_geometryShaders.insert({ shader.GetFileName(), std::make_shared<GeometryShader>(shader) });
		return inserted.first->second;
	}


	std::shared_ptr<VertexShader>& GetVertexShader(const std::wstring& filename)
	{
		ALWAYS_ASSERT(m_vertexShaders.find(filename) != m_vertexShaders.end() && "Wrong VertexShader Name");
		return m_vertexShaders.find(filename)->second;
	}
	std::shared_ptr<PixelShader>& GetPixelShader(const std::wstring& filename)
	{
		ALWAYS_ASSERT(m_pixelShaders.find(filename) != m_pixelShaders.end() && "Wrong PixelShader Name");
		return m_pixelShaders.find(filename)->second;
	}
	std::shared_ptr<HullShader>& GetHullShader(const std::wstring& filename)
	{
		ALWAYS_ASSERT(m_hullShaders.find(filename) != m_hullShaders.end() && "Wrong HullShader Name");
		return m_hullShaders.find(filename)->second;
	}
	std::shared_ptr<DomainShader>& GetDomainShader(const std::wstring& filename)
	{
		ALWAYS_ASSERT(m_domainShaders.find(filename) != m_domainShaders.end() && "Wrong DomainShader Name");
		return m_domainShaders.find(filename)->second;
	}
	std::shared_ptr<GeometryShader>& GetGeometryShader(const std::wstring& filename)
	{
		ALWAYS_ASSERT(m_geometryShaders.find(filename) != m_geometryShaders.end() && "Wrong GeometryShader Name");
		return m_geometryShaders.find(filename)->second;
	}

	static ShadersManager& GetInstance()
	{
		static ShadersManager instance;
		return instance;
	}
	ShadersManager(const ShadersManager&) = delete;
	ShadersManager& operator=(const ShadersManager&) = delete;

private:
	std::unordered_map<std::wstring, std::shared_ptr<VertexShader>> m_vertexShaders = {};
	std::unordered_map<std::wstring, std::shared_ptr<PixelShader>> m_pixelShaders = {};
	std::unordered_map<std::wstring, std::shared_ptr<HullShader>> m_hullShaders = {};
	std::unordered_map<std::wstring, std::shared_ptr<DomainShader>> m_domainShaders = {};
	std::unordered_map<std::wstring, std::shared_ptr<GeometryShader>> m_geometryShaders = {};
	ShadersManager() {}
};