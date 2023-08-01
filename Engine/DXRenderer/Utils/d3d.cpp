#include "d3d.h"
#include <cstdint>

ID3D11Device5* s_device = nullptr;
ID3D11DeviceContext4* s_devcon = nullptr;
IDXGIFactory5* s_factory = nullptr;
ID3D11Debug* s_devdebug = nullptr;

extern "C"
{
    _declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    _declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

void D3D::Init()
{
	HRESULT result;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)m_factory.reset());
	ALWAYS_ASSERT(result == S_OK && "CreateDXGIFactory");

	result = m_factory->QueryInterface(__uuidof(IDXGIFactory5), (void**)m_factory5.reset());
	ALWAYS_ASSERT(result == S_OK && "Query IDXGIFactory5");

	{
		uint32_t index = 0;
		IDXGIAdapter1* adapter;
		while (m_factory5->EnumAdapters1(index++, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			LOG("GPU #" << index << desc.Description);
		}
	}

	// Init D3D Device & Context

	const D3D_FEATURE_LEVEL featureLevelRequested = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_11_0;
	result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG,
		&featureLevelRequested, 1, D3D11_SDK_VERSION, m_device.reset(), &featureLevelInitialized, m_devcon.reset());
	ALWAYS_ASSERT(result == S_OK && "D3D11CreateDevice");
	ALWAYS_ASSERT(featureLevelRequested == featureLevelInitialized && "D3D_FEATURE_LEVEL_11_0");

	result = m_device->QueryInterface(__uuidof(ID3D11Device5), (void**)m_device5.reset());
	ALWAYS_ASSERT(result == S_OK && "Query ID3D11Device5");

	result = m_devcon->QueryInterface(__uuidof(ID3D11DeviceContext4), (void**)m_devcon4.reset());
	ALWAYS_ASSERT(result == S_OK && "Query ID3D11DeviceContext4");

	result = m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)m_devdebug.reset());
	ALWAYS_ASSERT(result == S_OK && "Query ID3D11Debug");

	// Write global pointers

	s_devdebug = m_devdebug.ptr();
	s_factory = m_factory5.ptr();
	s_device = m_device5.ptr();
	s_devcon = m_devcon4.ptr();
}

void D3D::DeInit()
{
}
