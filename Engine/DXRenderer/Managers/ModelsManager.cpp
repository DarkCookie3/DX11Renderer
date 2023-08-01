#include "../Utils/WideCharConverter.h"
#include "TexturesManager.h"
#include "ModelsManager.h"
#include "MeshSystem.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>
#include <codecvt>
#include <locale>
#include <functional>
#include "../DXObjects/RasterizerState.h"
#include "../DXObjects/DepthStencilState.h"

using namespace Assimp;

const auto standardMat = std::make_shared<Material>(Material::CreateMat(
	std::shared_ptr<ShadersGroup>(),
	XMVECTOR{1, 1, 1, 1}
	));

bool LoadTextureFromPath(aiString& texturePath, const std::wstring& name, DxResPtr<ID3D11Texture2D>& texture)
{
	auto substr_start = 0;
	auto substr_end = name.find_last_of(L"/\\") + 1;
	std::wstring modelFolderName = name.substr(substr_start, substr_end - substr_start);

	std::wstring texPathWStr = s2ws(MODELS_DIR) + modelFolderName + s2ws(texturePath.C_Str());

	std::wstring texExtensionName = texPathWStr.substr(texPathWStr.find_last_of(L"."), texPathWStr.size() - 1);
	if (texExtensionName == L".dds" || texExtensionName == L".DDS")
	{
		auto& texturesManager = TexturesManager::GetInstance();
		texture = texturesManager.AddTextureOfModel(texPathWStr);
		return true;
	}
	return false;
}

void LoadMaterialSetTextures(const aiScene* assimpScene, aiMesh* srcMesh, const std::wstring& name, InstancedMesh<Vertex>& newMesh)
{
	aiMaterial* material = assimpScene->mMaterials[srcMesh->mMaterialIndex];
	aiString texturePath;

	auto meshMaterial = std::make_shared<Material>(Material::CreateMat(
		std::shared_ptr<ShadersGroup>(),
		XMVECTOR{ 1, 1, 1, 1 }
		));

	bool hasAnyTexture = false;
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
	{
		if (LoadTextureFromPath(texturePath, name, meshMaterial->m_albedo))
		{
			hasAnyTexture = true;
		}
	}
	if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS)
	{
		if (LoadTextureFromPath(texturePath, name, meshMaterial->m_normal))
		{
			hasAnyTexture = true;
		}
	}
	if (material->GetTexture(aiTextureType_SHININESS, 0, &texturePath) == AI_SUCCESS)
	{
		if (LoadTextureFromPath(texturePath, name, meshMaterial->m_roughness))
		{
			hasAnyTexture = true;
		}
	}
	if (material->GetTexture(aiTextureType_METALNESS, 0, &texturePath) == AI_SUCCESS)
	{
		if (LoadTextureFromPath(texturePath, name, meshMaterial->m_metalness))
		{
			hasAnyTexture = true;
		}
	}
	if (material->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath) == AI_SUCCESS)
	{
		if (LoadTextureFromPath(texturePath, name, meshMaterial->m_emission))
		{
			hasAnyTexture = true;
		}
	}
	if (material->GetTexture(aiTextureType_HEIGHT, 0, &texturePath) == AI_SUCCESS)
	{
		if (LoadTextureFromPath(texturePath, name, meshMaterial->m_height))
		{
			hasAnyTexture = true;
		}
	}
	if (material->GetTexture(aiTextureType_OPACITY, 0, &texturePath) == AI_SUCCESS)
	{
		if (LoadTextureFromPath(texturePath, name, meshMaterial->m_opacity))
		{
			hasAnyTexture = true;
		}
	}
	if (material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texturePath) == AI_SUCCESS)
	{
		if (LoadTextureFromPath(texturePath, name, meshMaterial->m_ambientOcclusion))
		{
			hasAnyTexture = true;
		}
	}

	if (!hasAnyTexture)
	{
		newMesh.SetDefaultMaterial(standardMat, OpacityMeshGroup::Opaque);
		newMesh.SetDefaultMaterial(standardMat, OpacityMeshGroup::Translucent);
	}
	else
	{
		newMesh.SetDefaultMaterial(meshMaterial, OpacityMeshGroup::Opaque);
		newMesh.SetDefaultMaterial(meshMaterial, OpacityMeshGroup::Translucent);
	}
}

std::shared_ptr<Model> ModelsManager::GetModel(const std::wstring& name)
{
	auto it = m_models.find(name);
	if (it != m_models.end())
	{
		return it->second;
	}
	else
	{
		return LoadModel(name);
	}
}

void ModelsManager::AddModel(const std::wstring& name, std::shared_ptr<Model> model)
{
	m_models.emplace(name, model);
	MeshSystem::GetInstance().ForcePushModel(model);
}

std::shared_ptr<Model> ModelsManager::LoadModel(const std::wstring& name)
{
	uint32_t flags = uint32_t(aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);
	// aiProcess_Triangulate - ensure that all faces are triangles and not polygonals, otherwise triangulare them
	// aiProcess_GenBoundingBoxes - automatically compute bounding box, though we could do that manually
	// aiProcess_ConvertToLeftHanded - Assimp assumes left-handed basis orientation by default, convert for Direct3D
	// aiProcess_CalcTangentSpace - computes tangents and bitangents, they are used in advanced lighting

	Assimp::Importer importer;
	std::string filename(name.begin(), name.end());
	std::string path = MODELS_DIR + filename;
	const aiScene* assimpScene = importer.ReadFile(path, flags);
	DEV_ASSERT(assimpScene);

	auto modelToInsert = std::make_shared<Model>();
	uint32_t numMeshes = assimpScene->mNumMeshes;

	// Load vertex data
	auto resultMeshes = std::make_shared<std::vector<InstancedMesh<Vertex>>>();

	for (uint32_t nm = 0; nm < numMeshes; ++nm)
	{
		auto& srcMesh = assimpScene->mMeshes[nm];

		std::vector<Vertex> vertices = {};
		std::vector<uint32_t> indices = {};
		std::vector<TriangleIndicesDX> triangles = {};

		for (uint32_t v = 0; v < srcMesh->mNumVertices; ++v)
		{
			auto& position = srcMesh->mVertices[v];
			auto& normal = srcMesh->mNormals[v];
			auto& tangent = srcMesh->mTangents[v];
			auto& bitangent = srcMesh->mBitangents[v];


			XMVECTOR tex_coord = {0, 0, 0, 0};
			if (srcMesh->mTextureCoords[0]) 
			{
				tex_coord.m128_f32[0] = srcMesh->mTextureCoords[0][v].x;
				tex_coord.m128_f32[1] = srcMesh->mTextureCoords[0][v].y;
			}

			Vertex vertex;
			vertex.Position = DirectX::XMVectorSet(position.x, position.y, position.z, 1);
			vertex.Normal = DirectX::XMVectorSet(normal.x, normal.y, normal.z, 0);
			vertex.Tangent = DirectX::XMVectorSet(tangent.x, tangent.y, tangent.z, 0);
			vertex.Bitangent = DirectX::XMVectorSet(bitangent.x, bitangent.y, bitangent.z, 0);
			vertex.TexCoords = tex_coord;

			vertices.emplace_back(vertex);
		}
		for (unsigned int i = 0; i < srcMesh->mNumFaces; i++)
		{
			aiFace face = srcMesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.emplace_back(face.mIndices[j]);
			}

			triangles.emplace_back(*reinterpret_cast<TriangleIndicesDX*>(face.mIndices));
		}

		auto& newMesh = resultMeshes->emplace_back(InstancedMesh<Vertex>(srcMesh->mName.C_Str(), vertices, indices, triangles));
		auto& box = srcMesh->mAABB;
		newMesh.m_boundingBox = { XMVectorSet(box.mMin.x, box.mMin.y, box.mMin.z, 0), XMVectorSet(box.mMax.x, box.mMax.y, box.mMax.z, 0) };

		LoadMaterialSetTextures(assimpScene, srcMesh, name, newMesh);
	}

	std::function<void(aiNode*)> loadInstances;
	loadInstances = [&loadInstances, &resultMeshes, &modelToInsert](aiNode* node)
	{
		auto nodeMat = node->mTransformation.Transpose();
		DirectX::XMMATRIX& nodeToParent = reinterpret_cast<DirectX::XMMATRIX&>(nodeMat);

		// The same node may contain multiple meshes in its space, referring to them by indices
		for (uint32_t i = 0; i < node->mNumMeshes; ++i)
		{
			uint32_t meshIndex = node->mMeshes[i];
			resultMeshes->at(meshIndex).m_instancesData.meshToModel = nodeToParent;
		}

		for (uint32_t i = 0; i < node->mNumChildren; ++i)
			loadInstances(node->mChildren[i]);
	};

	loadInstances(assimpScene->mRootNode);

	*modelToInsert = Model(resultMeshes);
	m_models.emplace(name, modelToInsert);
	MeshSystem::GetInstance().m_models.emplace_back(modelToInsert);
	return modelToInsert;
}
