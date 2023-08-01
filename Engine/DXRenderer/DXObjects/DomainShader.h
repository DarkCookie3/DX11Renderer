#pragma once
#include <string>
#include "../Utils/d3d.h"
#include <vector>

class DomainShader
{
public:
	DomainShader() = default;
	DomainShader(const LPCWSTR& filepath);
	~DomainShader();

	void Bind();
	std::wstring& GetFileName() { return m_filename; }

private:
	DxResPtr<ID3D11DomainShader> m_shader;
	std::wstring m_filename;
};