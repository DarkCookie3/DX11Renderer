#include "HullShader.h"

HullShader::HullShader(const LPCWSTR& filepath)
{
	DxResPtr<ID3D10Blob> HS;
	DxResPtr<ID3DBlob> errorMessage;
	HRESULT H = D3DCompileFromFile(filepath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "HShader", "hs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, HS.reset(), errorMessage.reset());
	
	char* pErrorMessage; if (H != S_OK) { pErrorMessage = (char*)errorMessage->GetBufferPointer(); }

	ALWAYS_ASSERT(H == S_OK && "CompileHullShader");

	H = s_device->CreateHullShader(HS->GetBufferPointer(), HS->GetBufferSize(), NULL, m_shader.reset());
	ALWAYS_ASSERT(H == S_OK && "CreateHullShader");

	std::wstring filepath_str = filepath;
	auto substr_start = filepath_str.find_last_of(L"/\\") + 1;
	auto substr_end = filepath_str.find_last_of(L"\.");
	m_filename = filepath_str.substr(substr_start, substr_end - substr_start);
}

HullShader::~HullShader()
{

}

void HullShader::Bind()
{
	s_devcon->HSSetShader(m_shader.ptr(), 0, 0);
}
