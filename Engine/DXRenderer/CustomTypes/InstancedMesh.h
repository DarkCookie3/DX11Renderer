#pragma once
#include <algorithm>
#include "../DXObjects/VertexBuffer.h"
#include "../DXObjects/IndexBuffer.h"
#include "Model.h"
#include "../Managers/Renderer.h"
#include <unordered_map>

using namespace DirectX;

struct PerInstance
{
	TransformDX_ID transformID;
	float m_creationTimeSec;
};

struct PerMaterial
{
	std::shared_ptr<Material> material;
	std::vector<PerInstance> instances;
};

struct PackedPerInstance
{
	DirectX::XMVECTOR color;
	DirectX::XMVECTOR emission;
	DirectX::XMMATRIX instance;
	DirectX::XMMATRIX instanceInv;
	std::array<int32_t, MAX_PBR_TEXTURES_PER_MATERIAL> TexArraySliceIndex;
	std::array<float, 2> texCoordScale;
	float creationTimeSec;
};

struct PerMesh
{
	PerMaterial perMaterialDefault[2];
	std::vector<PerMaterial> perMaterial[2];
	DirectX::XMMATRIX meshToModel;

	std::unordered_map<std::shared_ptr<ShadersGroup>, std::unordered_map<std::shared_ptr<PipelineState>, std::vector<PackedPerInstance>>> GetPackedInstancesData(PBR_SRVGroup& srv_out, uint32_t opacityIndex);
};

struct InstanceBuffer
{
	VertexBuffer m_buffer;
	size_t m_size;
	bool requiresSizeChange = true;
	bool requiresUpdate = false;
};

template <typename T>
class InstancedMesh
{
public:
	InstancedMesh(const std::string& name, const std::vector<T>& vertices, const std::vector<uint32_t>& indices, const std::vector<TriangleIndicesDX>& triangles, const XMMATRIX& meshToModel = XMMatrixIdentity())
		: m_name(name), m_vertices(vertices), m_indices(indices), m_triangles(triangles),
		m_VertexBuffer(vertices.data(), sizeOfVertices()),
		m_IndexBuffer(indices)
	{
		m_instancesData.meshToModel = meshToModel;
	}
	InstancedMesh(const std::string& name, const std::vector<T>& vertices, const std::vector<uint32_t>& indices, const XMMATRIX& meshToModel = XMMatrixIdentity())
		: m_name(name), m_vertices(vertices), m_indices(indices), m_triangles(indices.size() / 3),
		m_VertexBuffer(vertices.data(), sizeOfVertices()),
		m_IndexBuffer(indices)
	{
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			TriangleIndicesDX triangle;
			triangle.indices[0] = m_indices[i];
			triangle.indices[1] = m_indices[i + 1];
			triangle.indices[2] = m_indices[i + 2];
			m_triangles[i / 3] = triangle;
		}
		m_instancesData.meshToModel = meshToModel;
	}
	InstancedMesh(const InstancedMesh& arg)
	{
		m_name = arg.m_name;
		m_VertexBuffer = arg.m_VertexBuffer;
		m_IndexBuffer = arg.m_IndexBuffer;
		m_instancesData = arg.m_instancesData;
		m_instancesNum = arg.m_instancesNum;
		m_boundingBox = arg.m_boundingBox;
		m_vertices = arg.m_vertices;
		m_triangles = arg.m_triangles;
		m_indices = arg.m_indices;
	}
	InstancedMesh& operator=(const InstancedMesh& arg)
	{
		m_name = arg.m_name;
		m_VertexBuffer = arg.m_VertexBuffer;
		m_IndexBuffer = arg.m_IndexBuffer;
		m_instancesData = arg.m_instancesData;
		m_instancesNum = arg.m_instancesNum;
		m_boundingBox = arg.m_boundingBox;
		m_vertices = arg.m_vertices;
		m_triangles = arg.m_triangles;
		m_indices = arg.m_indices;
		return *this;
	}

	std::string& GetName() { return m_name; }

	void SetBoundingBox(const BoxDX& box) { m_boundingBox = box; }
	void InitOctree() { m_octree.initialize(*this); }

	void AddInstanceWithMaterial(std::shared_ptr<Material> material, std::shared_ptr<Model> model, OpacityMeshGroup opacityGroup)
	{
		const auto opacityIndex = GetIndexForOpacityMeshGroup(opacityGroup);
		for (auto& singleMaterial : m_instancesData.perMaterial[opacityIndex])
		{
			if (singleMaterial.material.get() == material.get())
			{
				singleMaterial.instances.push_back({ model->GetTransformID(), model->GetCreationTime() });
				m_instancesNum[opacityIndex]++;
				m_InstanceBuffer[opacityIndex].requiresSizeChange = true;
				return;
			}
		}
		auto& newMat = m_instancesData.perMaterial[opacityIndex].emplace_back();
		newMat.material = material;
		newMat.instances.push_back({ model->GetTransformID(), model->GetCreationTime()});
		m_instancesNum[opacityIndex]++;
		m_InstanceBuffer[opacityIndex].requiresSizeChange = true;
	}

	void AddCopyInstance(std::shared_ptr<Model> srcModel, std::shared_ptr<Model> newModel)
	{
		for (int opacityIndex = 0; opacityIndex < 2; opacityIndex++)
		{
			for (auto& singleMaterial : m_instancesData.perMaterial[opacityIndex])
			{
				for (auto& instance : singleMaterial.instances)
				{
					if (instance.transformID == srcModel->GetTransformID())
					{
						singleMaterial.instances.push_back({ newModel->GetTransformID(), newModel->GetCreationTime() });
						m_instancesNum[opacityIndex]++;
						m_InstanceBuffer[opacityIndex].requiresSizeChange = true;
						return;
					}
				}
			}

			for (auto& instance : m_instancesData.perMaterialDefault[opacityIndex].instances)
			{
				if (instance.transformID == srcModel->GetTransformID())
				{
					m_instancesData.perMaterialDefault[opacityIndex].instances.push_back({ newModel->GetTransformID(), newModel->GetCreationTime() });
					m_instancesNum[opacityIndex]++;
					m_InstanceBuffer[opacityIndex].requiresSizeChange = true;
					return;
				}
			}
		}
	}

	void AddInstanceToDefaultMaterial(std::shared_ptr<Model> model, OpacityMeshGroup opacityGroup)
	{
		const auto opacityIndex = GetIndexForOpacityMeshGroup(opacityGroup);
		if (!m_instancesData.perMaterialDefault[opacityIndex].material)
		{
			ALWAYS_ASSERT(false && "No default material was previously provided for this instanced mesh");
		}
		m_instancesData.perMaterialDefault[opacityIndex].instances.push_back({ model->GetTransformID(), model->GetCreationTime() });
		m_instancesNum[opacityIndex]++;
		m_InstanceBuffer[opacityIndex].requiresSizeChange = true;
		return;
	}

	void ChangeShaders(Model* model, std::shared_ptr<ShadersGroup> newShaders)
	{
		std::vector<PerInstance>::iterator resultIter;
		auto searchPredicate = [&model](PerInstance& instance) { return instance.transformID == model->GetTransformID(); };

		for (int opacityIndex = 0; opacityIndex < 2; opacityIndex++)
		{
			if ((resultIter = std::find_if(m_instancesData.perMaterialDefault[opacityIndex].instances.begin(), m_instancesData.perMaterialDefault[opacityIndex].instances.end(), searchPredicate)) != m_instancesData.perMaterialDefault[opacityIndex].instances.end())
			{
				std::shared_ptr<Material> newMaterial = std::make_shared<Material>(*m_instancesData.perMaterialDefault[opacityIndex].material);
				newMaterial->shaders = newShaders;

				PerInstance perInstanceDataNew = { resultIter->transformID, resultIter->m_creationTimeSec };
				m_instancesData.perMaterialDefault[opacityIndex].instances.erase(resultIter);

				auto& newMat = m_instancesData.perMaterial[opacityIndex].emplace_back();
				newMat.material = newMaterial;
				newMat.instances.push_back(perInstanceDataNew);

				m_InstanceBuffer[opacityIndex].requiresSizeChange = true;
				return;
			}

			for (auto& singleMaterial : m_instancesData.perMaterial[opacityIndex])
			{
				if ((resultIter = std::find_if(singleMaterial.instances.begin(), singleMaterial.instances.end(), searchPredicate)) != singleMaterial.instances.end())
				{
					std::shared_ptr<Material> newMaterial = std::make_shared<Material>(*singleMaterial.material);
					newMaterial->shaders = newShaders;

					PerInstance perInstanceDataNew = { resultIter->transformID, resultIter->m_creationTimeSec };
					singleMaterial.instances.erase(resultIter);

					auto& newMat = m_instancesData.perMaterial[opacityIndex].emplace_back();
					newMat.material = newMaterial;
					newMat.instances.push_back(perInstanceDataNew);

					m_InstanceBuffer[opacityIndex].requiresSizeChange = true;
					return;
				}
			}
		}
	}

	void DeleteInstance(Model* model)
	{
		std::vector<PerInstance>::iterator resultIter;
		auto searchPredicate = [&model](PerInstance& instance) { return instance.transformID == model->GetTransformID(); };

		for (int opacityIndex = 0; opacityIndex < 2; opacityIndex++)
		{
			if ((resultIter = std::find_if(m_instancesData.perMaterialDefault[opacityIndex].instances.begin(), m_instancesData.perMaterialDefault[opacityIndex].instances.end(), searchPredicate)) != m_instancesData.perMaterialDefault[opacityIndex].instances.end())
			{
				m_instancesData.perMaterialDefault[opacityIndex].instances.erase(resultIter);
				m_instancesNum[opacityIndex]--;
				m_InstanceBuffer[opacityIndex].requiresSizeChange = true;
				return;
			}
		
			for (auto& singleMaterial : m_instancesData.perMaterial[opacityIndex])
			{
				if ((resultIter = std::find_if(singleMaterial.instances.begin(), singleMaterial.instances.end(), searchPredicate)) != singleMaterial.instances.end())
				{
					singleMaterial.instances.erase(resultIter);
					m_instancesNum[opacityIndex]--;
					m_InstanceBuffer[opacityIndex].requiresSizeChange = true;
					return;
				}
			}
		}
	}

	void SetDefaultMaterial(std::shared_ptr<Material> material, OpacityMeshGroup opacityGroup)
	{
		const auto opacityIndex = GetIndexForOpacityMeshGroup(opacityGroup);
		m_instancesData.perMaterialDefault[opacityIndex].material = material;
	}

	void SetDefaultShaders(std::shared_ptr<ShadersGroup> shaderGroup, OpacityMeshGroup opacityGroup)
	{
		const auto opacityIndex = GetIndexForOpacityMeshGroup(opacityGroup);
		m_instancesData.perMaterialDefault[opacityIndex].material->shaders = shaderGroup;
	}

	void SetDefaultPipelineState(std::shared_ptr<PipelineState> pipelineState, OpacityMeshGroup opacityGroup)
	{
		const auto opacityIndex = GetIndexForOpacityMeshGroup(opacityGroup);
		m_instancesData.perMaterialDefault[opacityIndex].material->m_pipelineState = pipelineState;
	}

	void MarkBuffersForUpdate()
	{
		m_InstanceBuffer[0].requiresUpdate = true;
		m_InstanceBuffer[1].requiresUpdate = true;
	}

	void BindShaders(std::shared_ptr<ShadersGroup> shaderGroup)
	{
		if (shaderGroup->vertexShader)
		{
			shaderGroup->vertexShader->Bind();
		}
		else
		{
			ALWAYS_ASSERT(false && "NO VERTEX SHADER PROVIDED");
		}

		if (shaderGroup->pixelShader)
		{
			shaderGroup->pixelShader->Bind();
		}
		else
		{
			ALWAYS_ASSERT(false && "NO PIXEL SHADER PROVIDED");
		}

		if (shaderGroup->hullShader)
		{
			shaderGroup->hullShader->Bind();
			s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		}
		else
		{
			s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			s_devcon->HSSetShader(nullptr, nullptr, 0);
		}

		if (shaderGroup->domainShader)
		{
			shaderGroup->domainShader->Bind();
		}
		else
		{
			s_devcon->DSSetShader(nullptr, nullptr, 0);
		}

		if (shaderGroup->geometryShader)
		{
			shaderGroup->geometryShader->Bind();
		}
		else
		{
			s_devcon->GSSetShader(nullptr, nullptr, 0);
		}
	}

	void Render(OpacityMeshGroup opacityGroup, bool bindTextures = true, bool bindShaders = true, bool bindState = true)
	{
		const auto opacityIndex = GetIndexForOpacityMeshGroup(opacityGroup);
		if (m_instancesNum[opacityIndex] == 0)
		{
			return;
		}
		else if (m_InstanceBuffer[opacityIndex].requiresSizeChange == true)
		{
			shadersGroupsStrides[opacityIndex].clear();
			auto instancesPerShadersGroups = m_instancesData.GetPackedInstancesData(m_shaderResourceViews[opacityIndex], opacityIndex);
			std::vector<PackedPerInstance> perInstanceDataPacked;
			for (auto& perShadersGroup : instancesPerShadersGroups)
			{
				for (auto& perPipelineState : perShadersGroup.second)
				{
					for (auto& instance : perPipelineState.second)
					{
						shadersGroupsStrides[opacityIndex][perShadersGroup.first][perPipelineState.first] += 1;
						perInstanceDataPacked.emplace_back(instance);
					}
				}
			}
			m_InstanceBuffer[opacityIndex].m_size = perInstanceDataPacked.size() * sizeof(PackedPerInstance);
			m_InstanceBuffer[opacityIndex].m_buffer = VertexBuffer(perInstanceDataPacked.data(), m_InstanceBuffer[opacityIndex].m_size, false);
			m_InstanceBuffer[opacityIndex].requiresSizeChange = false;
		}
		else if (m_InstanceBuffer[opacityIndex].requiresUpdate == true)
		{
			auto instancesPerShadersGroups = m_instancesData.GetPackedInstancesData(m_shaderResourceViews[opacityIndex], opacityIndex);
			std::vector<PackedPerInstance> perInstanceDataPacked;
			for (auto& perShadersGroup : instancesPerShadersGroups)
			{
				for (auto& perShadersGroup : instancesPerShadersGroups)
				{
					for (auto& perPipelineState : perShadersGroup.second)
					{
						for (auto& instance : perPipelineState.second)
						{
							perInstanceDataPacked.emplace_back(instance);
						}
					}
				}
			}
			m_InstanceBuffer[opacityIndex].m_buffer.Update(perInstanceDataPacked.data(), m_InstanceBuffer[opacityIndex].m_size);
			m_InstanceBuffer[opacityIndex].requiresUpdate = false;
		}
		
		m_VertexBuffer.Bind(0, sizeof(T), 0);
		m_IndexBuffer.Bind();
		m_InstanceBuffer[opacityIndex].m_buffer.Bind(1, m_InstanceBuffer[opacityIndex].m_size / m_instancesNum[opacityIndex], 0);

		if (bindTextures)
		{
			m_shaderResourceViews[opacityIndex].Bind(0);
		}
		
		int instanceOffset = 0;
		for (auto& shaderGroupOffset : shadersGroupsStrides[opacityIndex])
		{
			if (bindShaders)
			{
				BindShaders(shaderGroupOffset.first);
			}
			else
			{
				s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			}

			for (auto& perStateOffset : shaderGroupOffset.second)
			{
				if (bindState)
				{
					perStateOffset.first->m_depthStencilState.Bind(perStateOffset.first->stencilRefValue);
					perStateOffset.first->m_rasterizerState.Bind();
				}

				s_devcon->DrawIndexedInstanced(m_indices.size(), perStateOffset.second, 0, 0, instanceOffset);

				auto& renderer = Renderer::GetInstance();
				if (renderer.IsDebugEnabled())
				{
					renderer.BindDebugShader();
					s_devcon->DrawIndexedInstanced(m_indices.size(), perStateOffset.second, 0, 0, instanceOffset);
				}
				instanceOffset += perStateOffset.second;
			}
		}
	}

	bool Hit(const RayDX& ray, MeshIntersection& nearest);

	size_t sizeOfVertices()
	{
		return m_vertices.size() * sizeof(T);
	}

	friend class TriangleOctreeDX;
	friend class ModelsManager;
	friend class MeshSystem;
private:
	bool HitOneInstance(PerInstance& instance, const RayDX& ray, MeshIntersection& nearest);

	std::string m_name;

	std::vector<T> m_vertices;
	std::vector<TriangleIndicesDX> m_triangles;
	std::vector<uint32_t> m_indices;

	BoxDX m_boundingBox;
	TriangleOctreeDX m_octree;

	std::array<uint32_t, 2> m_instancesNum = {0, 0};
	PerMesh m_instancesData;

	VertexBuffer m_VertexBuffer;
	std::array<InstanceBuffer, 2> m_InstanceBuffer;
	IndexBuffer m_IndexBuffer;
	PBR_SRVGroup m_shaderResourceViews[2];

	std::array<std::unordered_map<std::shared_ptr<ShadersGroup>, std::unordered_map<std::shared_ptr<PipelineState>, uint32_t>>, 2> shadersGroupsStrides;
};
