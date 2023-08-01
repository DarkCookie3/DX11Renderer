#pragma once
#include "../DXObjects/ShaderResourceView.h"

#define MAX_PBR_TEXTURES_PER_MATERIAL 8

struct PBR_SRVGroup
{
	ShaderResourceView m_albedo;
	ShaderResourceView m_normal;
	ShaderResourceView m_roughness;
	ShaderResourceView m_metalness;
	ShaderResourceView m_specular;
	ShaderResourceView m_height;
	ShaderResourceView m_opacity;
	ShaderResourceView m_ambientOcclusion;

	void Bind(uint32_t startSlot)
	{
		m_albedo.Bind(startSlot++);
		m_normal.Bind(startSlot++);
		m_roughness.Bind(startSlot++);
		m_metalness.Bind(startSlot++);
		m_specular.Bind(startSlot++);
		m_height.Bind(startSlot++);
		m_opacity.Bind(startSlot++);
		m_ambientOcclusion.Bind(startSlot++);
	}
	ShaderResourceView& operator[](uint32_t index)
	{
		if (index >= MAX_PBR_TEXTURES_PER_MATERIAL || index < 0)
		{
			DEV_ASSERT(false && "TEXTURE INDEX IS OUT OF MAX_PBR_TEXTURES_PER_MATERIAL NUM");
			return m_albedo;
		}
		else
		{
			switch (index)
			{
			case 0:
				return m_albedo;
			case 1:
				return m_normal;
			case 2:
				return m_roughness;
			case 3:
				return m_metalness;
			case 4:
				return m_specular;
			case 5:
				return m_height;
			case 6:
				return m_opacity;
			case 7:
				return m_ambientOcclusion;
			}
		}
	}
};