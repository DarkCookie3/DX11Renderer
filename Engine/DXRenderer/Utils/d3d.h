#pragma once

#include "win_def.h"

#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include "../Utils/DirectXTex/DirectXTex.h"
#include "../Utils/DxTexLoader/DDSTextureLoader11.h"

#pragma comment( lib, "user32" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

// global pointers to most used D3D11 objects for convenience:
extern ID3D11Device5* s_device;
extern ID3D11DeviceContext4* s_devcon;
extern IDXGIFactory5* s_factory;
extern ID3D11Debug* s_devdebug;

#include "win_undef.h"

#include "DxRes.h"
#include "../../Common/AssertMacros.h"
#include "../../Common/LogMacros.h"

class D3D // a singletone for accessing global rendering resources
{
public:
	static D3D& GetInstance()
	{
		static D3D singleton;
		return singleton;
	}
	D3D(D3D const&) = delete;
	void operator=(D3D const&) = delete;

	void Init();
	void DeInit();

private:
	D3D() {}

	DxResPtr<IDXGIFactory> m_factory;
	DxResPtr<IDXGIFactory5> m_factory5;
	DxResPtr<ID3D11Device> m_device;
	DxResPtr<ID3D11Device5> m_device5;
	DxResPtr<ID3D11DeviceContext> m_devcon;
	DxResPtr<ID3D11DeviceContext4> m_devcon4;
	DxResPtr<ID3D11Debug> m_devdebug;
};
