#include "Model.h"
#include "InstancedMesh.h"
#include "../Managers/LightSystem.h"
#include "../../Common/Engine.h"

using namespace DirectX;

void Model::Render(OpacityMeshGroup groupsToDraw, bool useExternalTextures, bool useExternalShader, bool useExternalPipelineState)
{
	for (auto& mesh : *m_meshes)
	{
		mesh.Render(groupsToDraw, !useExternalTextures, !useExternalShader, !useExternalPipelineState);
	}
}

void Model::ChangeShaders(std::shared_ptr<ShadersGroup> shadersGroup)
{
	for (auto& mesh : *m_meshes)
	{
		mesh.ChangeShaders(this, shadersGroup);
	}
}

void Model::SetDefaultShadersForAllMeshes(std::shared_ptr<ShadersGroup> shaderGroup, OpacityMeshGroup opacityGroup)
{
	for (auto& mesh : *m_meshes)
	{
		mesh.SetDefaultShaders(shaderGroup, opacityGroup);
	}
}

void Model::SetDefaultPipelineStateForAllMeshes(std::shared_ptr<PipelineState> pipelineState, OpacityMeshGroup opacityGroup)
{
	for (auto& mesh : *m_meshes)
	{
		mesh.SetDefaultPipelineState(pipelineState, opacityGroup);
	}
}


void Model::InitOctree()
{
	for (auto& mesh : *m_meshes)
	{
		mesh.InitOctree();
	}
}

bool Model::Hit(const RayDX& ray, MeshIntersection& nearest)
{
	bool hit = false;

	for (auto& mesh : *m_meshes)
	{
		if (mesh.Hit(ray, nearest))
		{
			hit = true;
			nearest.model = this;
		}
	}
	return hit;
}

void Model::DeleteInstance()
{
	for (auto& mesh : *this->m_meshes)
	{
		mesh.DeleteInstance(this);
	}
}

Model::Model() : m_transformID(TransformDX::IdentityTransform()), m_meshes(std::make_shared<std::vector<InstancedMesh<Vertex>>>(std::vector<InstancedMesh<Vertex>>{}))
{
}

Model::Model(const shared_meshes_vector& meshes) : m_transformID(TransformDX::IdentityTransform()), m_meshes(meshes)
{
}

Model::Model(const shared_meshes_vector& meshes, const TransformDX& transform, float creationTimeSec) : m_transformID(transform), m_meshes(meshes), m_creationTimeSec(creationTimeSec)
{
}

std::shared_ptr<Model> Model::AddInstance(const TransformDX& transform, std::shared_ptr<Material> material, OpacityMeshGroup opacityGroup)
{
	auto result = std::make_shared<Model>(m_meshes, transform, Engine::GetInstance().GetTimer().GetCounted());
	for (auto& mesh : *m_meshes)
	{
		mesh.AddInstanceWithMaterial(material, result, opacityGroup);
	}
	return result;
}

std::shared_ptr<Model> Model::AddInstanceWithSameMaterial(const TransformDX& transform)
{
	auto result = std::make_shared<Model>(m_meshes, transform, Engine::GetInstance().GetTimer().GetCounted());
	for (auto& mesh : *m_meshes)
	{
		mesh.AddCopyInstance(std::shared_ptr<Model>(this), result);
	}
	return result;
}

std::shared_ptr<Model> Model::AddInstanceWithDefaultMaterial(const TransformDX& transform, OpacityMeshGroup opacityGroup)
{
	auto result = std::make_shared<Model>(m_meshes, transform, Engine::GetInstance().GetTimer().GetCounted());
	for (auto& mesh : *m_meshes)
	{
		mesh.AddInstanceToDefaultMaterial(result, opacityGroup);
	}
	return result;
}

std::shared_ptr<Model> Model::GetDeepCopy()
{
	return std::make_shared<Model>(std::make_shared<std::vector<InstancedMesh<Vertex>>>(*m_meshes),
								   GetTransform(), m_creationTimeSec);
}

bool Model::GetMeshByName(const std::string& name, InstancedMesh<Vertex>& out_mesh)
{
	std::vector<InstancedMesh<Vertex>>::iterator resultIter;
	auto searchPredicate = [&name](InstancedMesh<Vertex>& obj) {return obj.GetName() == name; };
	if ((resultIter = std::find_if(m_meshes->begin(), m_meshes->end(), searchPredicate)) != m_meshes->end())
	{
		out_mesh = *resultIter;
		return true;
	}
	return false;
}

void Model::Move(const XMVECTOR& offset, TransformDX_ID m_transformID)
{
	m_transformID.GetTransform().position += offset;
	for (auto& mesh : *m_meshes)
	{
		mesh.MarkBuffersForUpdate();
	}
}
