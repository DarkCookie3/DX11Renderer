#pragma once
#include "../CustomTypes/Material.h"

class MaterialsManager
{
public:
	static MaterialsManager& GetInstance()
	{
		static MaterialsManager instance;
		return instance;
	}
	MaterialsManager(const MaterialsManager&) = delete;
	MaterialsManager& operator=(const MaterialsManager&) = delete;

	void AddMaterial(const std::wstring& name, std::shared_ptr<Material> material);
	bool GetMaterial(const std::wstring& name, std::shared_ptr<Material>& outMaterial);
private:
	std::unordered_map<std::wstring, std::shared_ptr<Material>> m_materials = {};

	MaterialsManager() {}
};