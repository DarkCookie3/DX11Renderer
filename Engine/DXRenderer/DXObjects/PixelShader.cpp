#include "PixelShader.h"

PixelShader::PixelShader(const LPCWSTR& filepath)
{
	DxResPtr<ID3D10Blob> PS;
	DxResPtr<ID3DBlob> errorMessage;

	HRESULT H = D3DCompileFromFile(filepath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PShader", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, PS.reset(), errorMessage.reset());
	char* pErrorMessage; if (H != S_OK) { pErrorMessage = (char*)errorMessage->GetBufferPointer(); }
	ALWAYS_ASSERT(H == S_OK && "CompilePixelShader");
	
	H = s_device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, m_shader.reset());
	ALWAYS_ASSERT(H == S_OK && "CreatePixelShader");

	std::wstring filepath_str = filepath;
	auto substr_start = filepath_str.find_last_of(L"/\\") + 1;
	auto substr_end = filepath_str.find_last_of(L"\.");
	m_filename = filepath_str.substr(substr_start, substr_end - substr_start);
}

PixelShader::~PixelShader()
{
}

void PixelShader::Bind()
{
	s_devcon->PSSetShader(m_shader.ptr(), 0, 0);
}
