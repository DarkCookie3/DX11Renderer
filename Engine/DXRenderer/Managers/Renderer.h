#pragma once
#include "../DXObjects/VertexShader.h"
#include "../DXObjects/PixelShader.h"
#include "../DXObjects/DomainShader.h"
#include "../DXObjects/GeometryShader.h"
#include "../DXObjects/HullShader.h"
#include "../DXObjects/ConstantBuffer.h"
#include "../Window/WindowDX.h"
#include "../CustomTypes/CameraDX.h"
#include "../DXObjects/SamplerState.h"
#include "../CustomTypes/Skybox.h"
#include <typeindex>
#include <map>
#include "../DXObjects/BlendState.h"


enum class BufferSlots
{
	PER_FRAME = 0u,
	PER_VIEW = 1u,
	ADDITIONAL_PER_VIEW = 2u,
	IBL_GEN = 3u
};

struct ConstBufferWithActiveFlag
{
	ConstantBuffer m_constantBuffer;
	bool active = false;
};

struct SamplerStateWithActiveFlag
{
	SamplerState m_samplerState;
	bool active = false;
};

struct DebugDrawInfo
{
	bool enabled = false;
	std::shared_ptr<VertexShader> m_vertexShader;
	std::shared_ptr<PixelShader> m_pixelShader;
	std::shared_ptr<HullShader> m_hullShader;
	std::shared_ptr<DomainShader> m_domainShader;
	std::shared_ptr<GeometryShader> m_geometryShader;
};

class Renderer
{
public:
	static Renderer& GetInstance()
	{
		static Renderer instance;
		return instance;
	}
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	template <typename T>
	void InitConstBufferAtSlot(BufferSlots slotEnum, const T* const& data, size_t size)
	{
		auto slot = static_cast<uint32_t>(slotEnum);
		if (slot >= D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT)
		{
			ALWAYS_ASSERT(false && "ConstantBuffer slot is out of available range");
			return;
		}
		m_constantBuffers[slot].m_constantBuffer = ConstantBuffer(data, size);
		m_constantBuffers[slot].active = true;
		std::type_index typeID = std::type_index(typeid(T));
		m_constBuffersDataMap[typeID] = std::static_pointer_cast<void>(std::make_shared<T>(*data));
	}
	template <typename T>
	void UpdateConstBufferAtSlot(BufferSlots slotEnum)
	{
		auto slot = static_cast<uint32_t>(slotEnum);
		if (slot >= D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT)
		{
			ALWAYS_ASSERT(false && "ConstantBuffer slot is out of available range");
			return;
		}
		else if (!m_constantBuffers[slot].active)
		{
			ALWAYS_ASSERT(false && "ConstantBuffer was not initialized");
			return;
		}
		std::type_index typeID = std::type_index(typeid(T));
		m_constantBuffers[slot].m_constantBuffer.Update(std::static_pointer_cast<T>(m_constBuffersDataMap[typeID]).get(), sizeof(T));
	}
	void InitSamplerAtSlot(uint32_t slot, const D3D11_SAMPLER_DESC& samplerDesc)
	{
		m_samplerStates[slot].m_samplerState = SamplerState(samplerDesc);
		m_samplerStates[slot].active = true;
	}
	void InitBlendState(const std::string& name, D3D11_BLEND_DESC blendDesc, std::array<float, 4> blendFactor = {1, 1, 1, 1}, UINT sampleMask = 0xffffff)
	{
		m_blendStates.insert({ name, BlendState(blendDesc, blendFactor, sampleMask) });
	}
	void BindBlendState(const std::string& name)
	{
		auto it = m_blendStates.find(name);
		if (it != m_blendStates.end())
		{
			it->second.Bind();
		}
	}

	template<typename T>
	std::shared_ptr<T> GetConstBufferData(BufferSlots slotEnum)
	{
		std::type_index typeID = std::type_index(typeid(T));

		if (m_constBuffersDataMap.find(typeID) == m_constBuffersDataMap.end()) {
			m_constBuffersDataMap[typeID] = std::make_shared<T>();
		}

		return std::static_pointer_cast<T>(m_constBuffersDataMap[typeID]);
	}

	void BindAllSamplersAndConstBuffers();
	bool IsDebugEnabled() { return m_debugDraw.enabled; }
	void SetDebugFlag(bool flag) { m_debugDraw.enabled = flag; }
	void SetDebugShaders(std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader, std::shared_ptr<HullShader> hullShader, std::shared_ptr<DomainShader> domainShader, std::shared_ptr<GeometryShader> geometryShader);
	void SetNoiseMap(DxResPtr<ID3D11Texture2D> texture);
	void InitSkybox(std::shared_ptr<VertexShader> vertexShader, std::shared_ptr<PixelShader> pixelShader, const std::wstring& name);
	void SetSkyboxCubemap(DxResPtr<ID3D11Texture2D> texture);
	void BindIrradianceMaps(uint32_t slotDiffuse, uint32_t slotSpecular, uint32_t slotBRDFSpecular);
	void BindDebugShader();
	void BindNoiseMap(uint32_t slot);
	void Draw(WindowDX& window);
	void DrawDeferred();

private:
	ConstBufferWithActiveFlag m_constantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT + 1];
	std::map<std::type_index, std::shared_ptr<void>> m_constBuffersDataMap;
	SamplerStateWithActiveFlag m_samplerStates[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];
	std::unordered_map<std::string, BlendState> m_blendStates;
	FLOAT m_clearColor[4] = { 0.153f, 0.184f, 0.25f, 1.0f };
	DebugDrawInfo m_debugDraw;
	Skybox m_skybox;
	ShaderResourceView m_diffuseIrradianceMap;
	ShaderResourceView m_specularIrradianceMap;
	ShaderResourceView m_specularBRDFPrecompute;
	ShaderResourceView m_noiseMap;

	Renderer() {}
};
