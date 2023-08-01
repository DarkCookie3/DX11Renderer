#include "VertexShader.h"

VertexShader::VertexShader(const LPCWSTR& filepath, std::vector<D3D11_INPUT_ELEMENT_DESC> desc)
{
	DxResPtr<ID3D10Blob> VS;
	DxResPtr<ID3DBlob> errorMessage;
	HRESULT H = D3DCompileFromFile(filepath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VShader", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, VS.reset(), errorMessage.reset());
	char* pErrorMessage; if (H != S_OK) { pErrorMessage = (char*)errorMessage->GetBufferPointer(); }

	ALWAYS_ASSERT(H == S_OK && "CompileVertexShader");

	H = s_device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, m_shader.reset());
	ALWAYS_ASSERT(H == S_OK && "CreateVertexShader");

	std::wstring filepath_str = filepath;
	auto substr_start = filepath_str.find_last_of(L"/\\") + 1;
	auto substr_end = filepath_str.find_last_of(L"\.");
	m_filename = filepath_str.substr(substr_start, substr_end - substr_start);

	if (desc.size() == 0)
	{
		m_inputLayout.reset(nullptr);
	}
	else
	{
		H = s_device->CreateInputLayout(desc.data(), desc.size(), VS->GetBufferPointer(), VS->GetBufferSize(), m_inputLayout.reset());
		ALWAYS_ASSERT(H == S_OK && "CreateInputLayout");
	}
}

VertexShader::~VertexShader()
{
}

void VertexShader::Bind()
{
	s_devcon->VSSetShader(m_shader.ptr(), 0, 0);
	s_devcon->IASetInputLayout(m_inputLayout.ptr());
}
