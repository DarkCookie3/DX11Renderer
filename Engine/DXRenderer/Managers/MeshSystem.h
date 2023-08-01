#pragma once
#include <vector>
#include "../CustomTypes/InstancedMesh.h"
#include "../CustomTypes/Vertex.h"
#include "../CustomTypes/Model.h"

class MeshSystem
{
public:
	static MeshSystem& GetInstance()
	{
		static MeshSystem instance;
		return instance;
	}
	MeshSystem(const MeshSystem&) = delete;
	MeshSystem& operator=(const MeshSystem&) = delete;

	void Render(OpacityMeshGroup groupsToDraw, bool useExternalTextures, bool useExternalShaders, bool useExternalPipelineState);
	void ForcePushModel(const std::shared_ptr<Model>& model);
	bool CheckRayIntersections(const RayDX& ray, MeshIntersection& result);

	friend class ModelsManager;
private:
	MeshSystem() {}

	std::vector<std::shared_ptr<Model>> m_models;
};