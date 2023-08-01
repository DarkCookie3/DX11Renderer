#include "GeometryShader.h"

GeometryShader::GeometryShader(const LPCWSTR& filepath)
{
	DxResPtr<ID3D10Blob> GS;
	DxResPtr<ID3DBlob> errorMessage;
	HRESULT H = D3DCompileFromFile(filepath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GShader", "gs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, GS.reset(), errorMessage.reset());
	char* pErrorMessage; if (H != S_OK) { pErrorMessage = (char*)errorMessage->GetBufferPointer(); }
	
	ALWAYS_ASSERT(H == S_OK && "CompileGeometryShader");

	H = s_device->CreateGeometryShader(GS->GetBufferPointer(), GS->GetBufferSize(), NULL, m_shader.reset());
	ALWAYS_ASSERT(H == S_OK && "CreateGeometryShader");

	std::wstring filepath_str = filepath;
	auto substr_start = filepath_str.find_last_of(L"/\\") + 1;
	auto substr_end = filepath_str.find_last_of(L"\.");
	m_filename = filepath_str.substr(substr_start, substr_end - substr_start);
}

GeometryShader::~GeometryShader()
{

}

void GeometryShader::Bind()
{
	s_devcon->GSSetShader(m_shader.ptr(), 0, 0);
}
