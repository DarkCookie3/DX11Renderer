#include "InstancedMesh.h"
#include "../Utils/d3d.h"
#include "Model.h"

std::array<int32_t, MAX_PBR_TEXTURES_PER_MATERIAL> GetPackedTextures(std::array<int32_t, MAX_PBR_TEXTURES_PER_MATERIAL>& texArrSliceIndexCounter, PerMaterial& material, std::array<std::vector<DxResPtr<ID3D11Texture2D>>, MAX_PBR_TEXTURES_PER_MATERIAL>& texturesPacked)
{
	std::array<int32_t, MAX_PBR_TEXTURES_PER_MATERIAL> texArrSliceIndices;
	texArrSliceIndices.fill(-1);

	if (material.material->m_albedo != nullptr)
	{
		texturesPacked[0].push_back(material.material->m_albedo);
		texArrSliceIndices[0] = texArrSliceIndexCounter[0]++;
	}
	if (material.material->m_normal != nullptr)
	{
		texturesPacked[1].push_back(material.material->m_normal);
		texArrSliceIndices[1] = texArrSliceIndexCounter[1]++;
	}
	if (material.material->m_roughness != nullptr)
	{
		texturesPacked[2].push_back(material.material->m_roughness);
		texArrSliceIndices[2] = texArrSliceIndexCounter[2]++;
	}
	if (material.material->m_metalness != nullptr)
	{
		texturesPacked[3].push_back(material.material->m_metalness);
		texArrSliceIndices[3] = texArrSliceIndexCounter[3]++;
	}
	if (material.material->m_emission != nullptr)
	{
		texturesPacked[4].push_back(material.material->m_emission);
		texArrSliceIndices[4] = texArrSliceIndexCounter[4]++;
	}
	if (material.material->m_height != nullptr)
	{
		texturesPacked[5].push_back(material.material->m_height);
		texArrSliceIndices[5] = texArrSliceIndexCounter[5]++;
	}
	if (material.material->m_opacity != nullptr)
	{
		texturesPacked[6].push_back(material.material->m_opacity);
		texArrSliceIndices[6] = texArrSliceIndexCounter[6]++;
	}
	if (material.material->m_ambientOcclusion != nullptr)
	{
		texturesPacked[7].push_back(material.material->m_ambientOcclusion);
		texArrSliceIndices[7] = texArrSliceIndexCounter[7]++;
	}
	return texArrSliceIndices;
}

PBR_SRVGroup CreateSRVsForTextures(std::array<std::vector<DxResPtr<ID3D11Texture2D>>, MAX_PBR_TEXTURES_PER_MATERIAL>& texturesPacked, std::unordered_map<std::shared_ptr<ShadersGroup>, std::unordered_map<std::shared_ptr<PipelineState>, std::vector<PackedPerInstance>>>& perInstanceDataPacked)
{
	PBR_SRVGroup resultSrvGroup;
	bool scaleCalculated = false;

	UINT maxWidth = 0;
	UINT maxHeight = 0;
	UINT maxMip = 1;
	for (int i = 0; i < MAX_PBR_TEXTURES_PER_MATERIAL; i++)
	{
		if (texturesPacked[i].size() > 0)
		{
			if (!scaleCalculated)
			{
				for (auto texture : texturesPacked[i])
				{
					D3D11_TEXTURE2D_DESC desc;
					texture->GetDesc(&desc);

					if (desc.Width > maxWidth)
						maxWidth = desc.Width;

					if (desc.Height > maxHeight)
						maxHeight = desc.Height;

					if (desc.MipLevels > maxMip)
						maxMip = desc.MipLevels;
				}

				for (auto& pair : perInstanceDataPacked)
				{
					for (auto& perPipelineState : pair.second)
					{
						for (auto& instanceData : perPipelineState.second)
						{
							if (instanceData.TexArraySliceIndex[i] >= 0)
							{
								auto& textureFromArray = texturesPacked[i][instanceData.TexArraySliceIndex[i]];
								D3D11_TEXTURE2D_DESC desc;
								textureFromArray->GetDesc(&desc);
								instanceData.texCoordScale[0] = desc.Width / float(maxWidth);
								instanceData.texCoordScale[1] = desc.Height / float(maxHeight);
							}
						}
					}
				}
				scaleCalculated = true;
			}

			D3D11_TEXTURE2D_DESC texArrayDesc;
			texturesPacked[i][0]->GetDesc(&texArrayDesc);
			texArrayDesc.Height = maxHeight;
			texArrayDesc.Width = maxWidth;
			texArrayDesc.MipLevels = maxMip;
			texArrayDesc.ArraySize = texturesPacked[i].size();

			DxResPtr<ID3D11Texture2D> texArray;
			s_device->CreateTexture2D(&texArrayDesc, nullptr, texArray.reset());

			for (int k = 0; k < texturesPacked[i].size(); k++)
			{
				D3D11_TEXTURE2D_DESC texDesc;
				texturesPacked[i][k]->GetDesc(&texDesc);
				for (int j = 0; j < texDesc.MipLevels; j++)
				{
					s_devcon->CopySubresourceRegion(texArray.ptr(), texArrayDesc.MipLevels * k + j, 0, 0, 0, texturesPacked[i][k], j, NULL);
				}
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = texArrayDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
			srvDesc.Texture2DArray.ArraySize = texturesPacked[i].size();

			DxResPtr<ID3D11Resource> resource;
			texArray->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(resource.reset()));

			resultSrvGroup[i] = ShaderResourceView(srvDesc, resource);
		}
	}
	return resultSrvGroup;
}

std::unordered_map<std::shared_ptr<ShadersGroup>, std::unordered_map<std::shared_ptr<PipelineState>, std::vector<PackedPerInstance>>> PerMesh::GetPackedInstancesData(PBR_SRVGroup& srv_out, uint32_t opacityIndex)
{
	std::unordered_map<std::shared_ptr<ShadersGroup>, std::unordered_map<std::shared_ptr<PipelineState>, std::vector<PackedPerInstance>>> perInstanceDataPacked;
	std::array<std::vector<DxResPtr<ID3D11Texture2D>>, MAX_PBR_TEXTURES_PER_MATERIAL> texturesPacked;
	std::array<int32_t, MAX_PBR_TEXTURES_PER_MATERIAL> texArrSliceIndexCounter;
	texArrSliceIndexCounter.fill(0);

	if(perMaterialDefault[opacityIndex].material)
	{
		auto texArrSliceIndices = GetPackedTextures(texArrSliceIndexCounter, perMaterialDefault[opacityIndex], texturesPacked);
		for (auto& instance : perMaterialDefault[opacityIndex].instances)
		{
			std::array<float, 2> texCoordMock = {1, 1};
			XMMATRIX modelMeshMat = meshToModel * instance.transformID.GetTransform().ToMat4();
			perInstanceDataPacked[perMaterialDefault[opacityIndex].material->shaders][perMaterialDefault[opacityIndex].material->m_pipelineState].push_back({
				perMaterialDefault[opacityIndex].material->albedoColor,
				perMaterialDefault[opacityIndex].material->emissionColor,
				modelMeshMat,
				XMMatrixInverse(0, modelMeshMat),
				texArrSliceIndices,
				texCoordMock,
				instance.m_creationTimeSec
				});
		}
	}
	for (auto& singleMaterial : perMaterial[opacityIndex])
	{
		auto texArrSliceIndices = GetPackedTextures(texArrSliceIndexCounter, singleMaterial, texturesPacked);
		for (auto& instance : singleMaterial.instances)
		{
			std::array<float, 2> texCoordMock = { 1, 1 };
			XMMATRIX modelMeshMat = meshToModel * instance.transformID.GetTransform().ToMat4();
			perInstanceDataPacked[singleMaterial.material->shaders][singleMaterial.material->m_pipelineState].push_back({
				singleMaterial.material->albedoColor,
				singleMaterial.material->emissionColor,
				modelMeshMat,
				XMMatrixInverse(0, modelMeshMat),
				texArrSliceIndices,
				texCoordMock,
				instance.m_creationTimeSec
				});
		}
	}

	srv_out = CreateSRVsForTextures(texturesPacked, perInstanceDataPacked);
	return perInstanceDataPacked;
}


bool InstancedMesh<Vertex>::HitOneInstance(PerInstance& instance, const RayDX& ray, MeshIntersection& nearest)
{
	auto combinedTransformMat = m_instancesData.meshToModel * instance.transformID.GetTransform().ToMat4();
	auto inv = XMMatrixInverse(nullptr, combinedTransformMat);
	auto newOrig = XMVector4Transform(ray.m_origin, inv);
	auto newDir = XMVector4Transform(ray.m_direction, inv);
	RayDX newRay;
	newRay.m_origin = newOrig;
	newRay.m_direction = newDir;
	if (m_octree.intersect(newRay, nearest))
	{
		nearest.pos = XMVector4Transform(nearest.pos, combinedTransformMat);
		nearest.normal = XMVector4Transform(nearest.normal, inv);
		nearest.instance = &instance;
		nearest.mesh = this;
		return true;
	}
	return false;
}

bool InstancedMesh<Vertex>::Hit(const RayDX& ray, MeshIntersection& nearest)
{
	bool hit = false;
	for (int opacityIndex = 0; opacityIndex < 2; opacityIndex++)
	{
		for (auto& singleMaterial : m_instancesData.perMaterial[opacityIndex])
		{
			for (auto& instance : singleMaterial.instances)
			{
				if (HitOneInstance(instance, ray, nearest))
				{
					hit = true;
				}
			}
		}
		for (auto& instance : m_instancesData.perMaterialDefault[opacityIndex].instances)
		{
			if (HitOneInstance(instance, ray, nearest))
			{
				hit = true;
			}
		}
	}

	return hit;
}