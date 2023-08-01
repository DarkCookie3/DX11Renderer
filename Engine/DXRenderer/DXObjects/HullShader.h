#pragma once
#include <string>
#include "../Utils/d3d.h"
#include <vector>

class HullShader
{
public:
	HullShader() = default;
	HullShader(const LPCWSTR& filepath);
	~HullShader();

	void Bind();
	std::wstring& GetFileName() { return m_filename; }

private:
	DxResPtr<ID3D11HullShader> m_shader;
	std::wstring m_filename;
};