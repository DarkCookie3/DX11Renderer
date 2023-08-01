#pragma once
#include <vector>
#include "../CustomTypes/PointLight.h"
#include "../CustomTypes/DirLight.h"
#include "../CustomTypes/SpotLight.h"
#include "../DXObjects/StructuredBuffer.h"

constexpr float SHADOW_MARGIN = 100.0f;

struct StructuredBufferWithFlag
{
	StructuredBuffer m_buffer;
	bool isArrangmentChanged = false;
};

class LightSystem
{
public:
	static LightSystem& GetInstance()
	{
		static LightSystem instance;
		return instance;
	}
	LightSystem(const LightSystem&) = delete;
	LightSystem& operator=(const LightSystem&) = delete;

	void AddPointLight(std::shared_ptr<Model> model, LightMaterial lightMat, const TransformDX& modelTransform, const TransformDX& lightTransform);
	void AddSpotLight(std::shared_ptr<Model> model, LightMaterial lightMaterial, const TransformDX& modelTransform, const TransformDX& lightTransform, float lightAngleRad, float fadeAngleRad);
	void AddDirLight(const TransformDX& lightTransform, LightMaterial lightMat);
	
	void AddPointLightToModel(std::shared_ptr<Model> model, LightMaterial lightMat, const TransformDX& lightTransform);

	void BindLightsSRV();

	SpotLight* GetZeroIndexedSpotLight() 
	{ 
		if (m_spotLights.size())
		{
			return &m_spotLights[0];
		}
		else
		{
			return nullptr;
		}
	}
private:
	LightSystem() {}

	void GenerateShadowMaps(std::vector<PackedDirLight>& packedDirLights, std::vector<PackedPointLight>& packedPointLights, std::vector<PackedSpotLight>& packedSpotLights);

	void GenerateDirShadowMaps(std::vector<PackedDirLight>& packedDirLights);
	void GeneratePointShadowMaps(std::vector<PackedPointLight>& packedPointLights);
	void GenerateSpotShadowMaps(std::vector<PackedSpotLight>& packedSpotLights);

	std::vector<PackedPointLight> GetPackedPointLights();
	std::vector<PackedDirLight> GetPackedDirLights();
	std::vector<PackedSpotLight> GetPackedSpotLights(ShaderResourceView& srv_out);

	std::vector<PointLight> m_pointLights;
	std::vector<DirLight> m_dirLights;
	std::vector<SpotLight> m_spotLights;

	ShaderResourceView m_dirShadowMaps;
	ShaderResourceView m_pointShadowMaps;
	ShaderResourceView m_spotShadowMaps;

	ShaderResourceView m_spotLightTextureMasks;
	
	StructuredBufferWithFlag pointLightSources;
	StructuredBufferWithFlag spotLightSources;
	StructuredBufferWithFlag dirLightSources;
};