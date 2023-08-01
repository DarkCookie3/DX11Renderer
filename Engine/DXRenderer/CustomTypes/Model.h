#pragma once
#include <vector>
#include "Vertex.h"
#include "../Managers/TransformSystem.h"
#include "OpacityMeshGroup.h"
#include "Material.h"
#include "TriangleIndicesDX.h"
#include "TriangleOctreeDX.h"
#include "PBR_SRVGroup.h"
#include "../Managers/ShadersManager.h"
#include "../CustomTypes/OpacityMeshGroup.h"


using shared_meshes_vector = std::shared_ptr<std::vector<InstancedMesh<Vertex>>>;

class Model
{
public:
	Model();
	Model(const shared_meshes_vector& meshes);
	Model(const shared_meshes_vector& meshes, const TransformDX& transform, float creationTimeSec);

	std::shared_ptr<Model> AddInstance(const TransformDX& transform, std::shared_ptr<Material> material, OpacityMeshGroup opacityGroup);
	std::shared_ptr<Model> AddInstanceWithSameMaterial(const TransformDX& transform);
	std::shared_ptr<Model> AddInstanceWithDefaultMaterial(const TransformDX& transform, OpacityMeshGroup opacityGroup);
	std::shared_ptr<Model> GetDeepCopy();
	bool GetMeshByName(const std::string& name, InstancedMesh<Vertex>& out_mesh);
	void Move(const XMVECTOR& offset, TransformDX_ID m_transformID);
	void Render(OpacityMeshGroup groupsToDraw, bool useExternalTextures, bool useExternalShaders, bool useExternalPipelineState);
	void ChangeShaders(std::shared_ptr<ShadersGroup> shadersGroup);
	void SetDefaultShadersForAllMeshes(std::shared_ptr<ShadersGroup> shaderGroup, OpacityMeshGroup opacityGroup);
	void SetDefaultPipelineStateForAllMeshes(std::shared_ptr<PipelineState> pipelineState, OpacityMeshGroup opacityGroup);
	void InitOctree();
	bool Hit(const RayDX& ray, MeshIntersection& nearest);
	TransformDX& GetTransform() { return m_transformID.GetTransform(); }
	TransformDX_ID GetTransformID() { return m_transformID; }
	float GetCreationTime() { return m_creationTimeSec; }
	void DeleteInstance();

	friend class ModelsManager;
private:
	TransformDX_ID m_transformID;
	float m_creationTimeSec = 0;
	shared_meshes_vector m_meshes;
};