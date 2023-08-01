#include "MaterialManager.h"

void MaterialsManager::AddMaterial(const std::wstring& name, std::shared_ptr<Material> material)
{
    auto inserted = m_materials.emplace(name, material);
}

bool MaterialsManager::GetMaterial(const std::wstring& name, std::shared_ptr<Material>& outMaterial)
{
	auto it = m_materials.find(name);
	if (it != m_materials.end())
	{
		outMaterial = it->second;
		return true;
	}
	return false;
}
