#pragma once
#include "../Utils/d3d.h"
#include <vector>

class VertexShader
{
public:
	VertexShader() = default;
	VertexShader(const LPCWSTR& filepath, std::vector<D3D11_INPUT_ELEMENT_DESC> desc);
	~VertexShader();

	void Bind();
	std::wstring& GetFileName() { return m_filename; }
private:
	DxResPtr<ID3D11VertexShader> m_shader;
	DxResPtr<ID3D11InputLayout> m_inputLayout;
	std::wstring m_filename;
};