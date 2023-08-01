#pragma once
#include <string>
#include "../CustomTypes/Model.h"
#include <unordered_map>

#define MODELS_DIR std::string("../../Engine/DXRenderer/Resources/Models/")

class ModelsManager
{
public:

	static ModelsManager& GetInstance()
	{
		static ModelsManager instance;
		return instance;
	}
	ModelsManager(const ModelsManager&) = delete;
	ModelsManager& operator=(const ModelsManager&) = delete;

	std::shared_ptr<Model> GetModel(const std::wstring& name);
	void AddModel(const std::wstring& name, std::shared_ptr<Model> model);
private:
	std::unordered_map<std::wstring, std::shared_ptr<Model>> m_models = {};
	std::shared_ptr<Model> LoadModel(const std::wstring& name);

	ModelsManager() {}
};