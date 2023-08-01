#include "MeshSystem.h"
#include "../../Common/Engine.h"

void MeshSystem::Render(OpacityMeshGroup groupsToDraw, bool useExternalTextures, bool useExternalShaders, bool useExternalPipelineState)
{
	for (auto& model : m_models)
	{
		model->Render(groupsToDraw, useExternalTextures, useExternalShaders, useExternalPipelineState);
	}
}

void MeshSystem::ForcePushModel(const std::shared_ptr<Model>& model)
{
	m_models.push_back(model);
}

bool MeshSystem::CheckRayIntersections(const RayDX& ray, MeshIntersection& result)
{
	bool hit = false;
	MeshIntersection temp;
	temp.reset(0.01f);
	for (auto& model : m_models)
	{
		if (model->Hit(ray, temp))
		{
			result = temp;
			hit = true;
		}
	}
	return hit;
}
