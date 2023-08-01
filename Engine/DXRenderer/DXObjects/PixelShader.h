#pragma once
#include "../Utils/d3d.h"

class PixelShader
{
public:
	PixelShader() = default;
	PixelShader(const LPCWSTR& filepath);
	~PixelShader();

	void Bind();
	std::wstring& GetFileName() { return m_filename; }

private:
	DxResPtr<ID3D11PixelShader> m_shader;
	std::wstring m_filename;
};