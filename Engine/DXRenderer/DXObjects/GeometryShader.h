#pragma once
#include <string>
#include "../Utils/d3d.h"
#include <vector>

class GeometryShader
{
public:
	GeometryShader() = default;
	GeometryShader(const LPCWSTR& filepath);
	~GeometryShader();

	void Bind();
	std::wstring& GetFileName() { return m_filename; }

private:
	DxResPtr<ID3D11GeometryShader> m_shader;
	std::wstring m_filename;
};