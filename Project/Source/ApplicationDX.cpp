#include "ApplicationDX.h"
#include <DXRenderer/DXObjects/VertexShader.h>
#include <DXRenderer/DXObjects/VertexBuffer.h>
#include <Common/Engine.h>
#include "DXRenderer/Managers/LightSystem.h"
#include "DXRenderer/Math/Cube.h"
#include "DXRenderer/Math/Sphere.h"
#include <DXRenderer/Managers/MeshSystem.h>
#include <DXRenderer/Managers/ModelsManager.h>
#include <DXRenderer/Managers/Renderer.h>
#include <DXRenderer/Managers/TexturesManager.h>
#include <DXRenderer/Managers/PostProcess.h>
#include <DXRenderer/Managers/ReflectionCapturer.h>
#include <DXRenderer/Managers/MaterialManager.h>
#include <DXRenderer/Managers/ParticleSystem.h>

ApplicationDX::ApplicationDX(HINSTANCE hInstance, LPCWSTR className, int x, int y, int width, int height) : m_window(hInstance, className, x, y, width, height)
{
}

ApplicationDX::~ApplicationDX()
{
}

void ApplicationDX::InitSetup()
{
	CameraDX camera = CameraDX({ 0, 2, -9, 1 }, { 0, 2, 1, 1 }, 0.1f, 100.0f, 60.0f);
	auto& engine = Engine::GetInstance();
	engine.SetInputHandler(std::bind(&ApplicationDX::ProcessInput, this, std::placeholders::_1));
	engine.SetWindowAndCamera(&m_window, camera, 10.0f, 4.f);

	auto& transformSystem = TransformSystem::GetInstance();
	auto& meshSystem = MeshSystem::GetInstance();
	auto& modelsManager = ModelsManager::GetInstance();
	auto& particleSystem = ParticleSystem::GetInstance();
	auto& texturesManager = TexturesManager::GetInstance();
	auto& materialsManager = MaterialsManager::GetInstance();

	DxResPtr<ID3D11Texture2D> testTexture;
	texturesManager.GetTexture(testTexture, L"chess.dds");
	DxResPtr<ID3D11Texture2D> graniteColor;
	DxResPtr<ID3D11Texture2D> graniteNormal;
	DxResPtr<ID3D11Texture2D> graniteRoughness;
	DxResPtr<ID3D11Texture2D> graniteMetal;
	texturesManager.GetTexture(graniteColor, L"graniteColor.dds");
	texturesManager.GetTexture(graniteNormal, L"graniteNormal.dds");
	texturesManager.GetTexture(graniteRoughness, L"graniteRoughness.dds");
	texturesManager.GetTexture(graniteMetal, L"graniteMetal.dds");
	DxResPtr<ID3D11Texture2D> testTexture2;
	texturesManager.GetTexture(testTexture2, L"Mip_Level_Vis.dds");
	DxResPtr<ID3D11Texture2D> spotlightMask;
	texturesManager.GetTexture(spotlightMask, L"SpotLightMask.dds");
	DxResPtr<ID3D11Texture2D> noiseTexture;
	texturesManager.GetTexture(noiseTexture, L"noise9.dds");
	DxResPtr<ID3D11Texture2D> skybox;
	texturesManager.AddCubemapTexture(skybox, L"grass_field.dds");
	DxResPtr<ID3D11Texture2D> smokeEMVA;
	DxResPtr<ID3D11Texture2D> smokeDBF;
	DxResPtr<ID3D11Texture2D> smokeRLU;
	texturesManager.GetTexture(smokeEMVA, L"smoke_MVEA.dds");
	texturesManager.GetTexture(smokeDBF, L"smoke_RLU.dds");
	texturesManager.GetTexture(smokeRLU, L"smoke_DBF.dds");

	std::vector<D3D11_INPUT_ELEMENT_DESC> iedInstanced
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BITANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"EMISSION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

		{"INSTANCE_MODEL_MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE_MODEL_MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE_MODEL_MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE_MODEL_MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"INSTANCE_MODEL_MATRIX_INV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE_MODEL_MATRIX_INV", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE_MODEL_MATRIX_INV", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 128, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE_MODEL_MATRIX_INV", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 144, D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"TEXARR_INDEX", 0, DXGI_FORMAT_R32_SINT, 1, 160, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXARR_INDEX", 1, DXGI_FORMAT_R32_SINT, 1, 164, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXARR_INDEX", 2, DXGI_FORMAT_R32_SINT, 1, 168, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXARR_INDEX", 3, DXGI_FORMAT_R32_SINT, 1, 172, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXARR_INDEX", 4, DXGI_FORMAT_R32_SINT, 1, 176, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXARR_INDEX", 5, DXGI_FORMAT_R32_SINT, 1, 180, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXARR_INDEX", 6, DXGI_FORMAT_R32_SINT, 1, 184, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXARR_INDEX", 7, DXGI_FORMAT_R32_SINT, 1, 188, D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"TEX_COORD_SCALE", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 192, D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"CREATION_TIME", 0, DXGI_FORMAT_R32_FLOAT, 1, 200, D3D11_INPUT_PER_INSTANCE_DATA, 1}

	};
	std::vector<D3D11_INPUT_ELEMENT_DESC> iedDecal
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BITANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{"INSTANCE_MODEL_MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{"INSTANCE_MODEL_MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{"INSTANCE_MODEL_MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{"INSTANCE_MODEL_MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

		{"INSTANCE_MODEL_MATRIX_INV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{"INSTANCE_MODEL_MATRIX_INV", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{"INSTANCE_MODEL_MATRIX_INV", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{"INSTANCE_MODEL_MATRIX_INV", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	std::vector<D3D11_INPUT_ELEMENT_DESC> iedParticle
	{
		{"INSTANCE_MODEL_MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE_MODEL_MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE_MODEL_MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE_MODEL_MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"COLOR_RGBA", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"COLOR_EMISSION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"ANIMATION_STATE", 0, DXGI_FORMAT_R32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};
	std::vector<D3D11_INPUT_ELEMENT_DESC> iedEmpty
	{
	};

	ShadersManager& shadersManager = ShadersManager::GetInstance();
	auto vertex_shader_instanced = shadersManager.AddVertexShader(L"../../Engine/DXRenderer/Resources/Shaders/Default/InstancedVertex.hlsl", iedInstanced);
	auto vertex_shader_skybox = shadersManager.AddVertexShader(L"../../Engine/DXRenderer/Resources/Shaders/Skybox/SkyboxVertex.hlsl", iedEmpty);
	auto vertex_pass_through_instanced = shadersManager.AddVertexShader(L"../../Engine/DXRenderer/Resources/Shaders/Default/InstancedVertexPassThrough.hlsl", iedInstanced);
	auto vertex_postprocess = shadersManager.AddVertexShader(L"../../Engine/DXRenderer/Resources/Shaders/PostProcess/PostProcessVertex.hlsl", iedEmpty);
	auto vertex_irradiance_gen = shadersManager.AddVertexShader(L"../../Engine/DXRenderer/Resources/Shaders/IrradianceMaps/IrradianceVertex.hlsl", iedEmpty);
	auto vertex_flat_shadow = shadersManager.AddVertexShader(L"../../Engine/DXRenderer/Resources/Shaders/Shadows/FlatShadowVertex.hlsl", iedInstanced);
	auto vertex_shadow_pass_through = shadersManager.AddVertexShader(L"../../Engine/DXRenderer/Resources/Shaders/Shadows/ShadowVertexPassThrough.hlsl", iedInstanced);
	auto vertex_particle = shadersManager.AddVertexShader(L"../../Engine/DXRenderer/Resources/Shaders/Particles/ParticleVertex.hlsl", iedParticle);
	auto pixel_shader_holo = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/Default/HolographicPixel.hlsl");
	auto pixel_solid_color = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/Normals/PixelSolidColor.hlsl");
	auto pixel_skybox = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/Skybox/SkyboxPixel.hlsl");
	auto pixel_light = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/LightSources/LightSourcePixel.hlsl");
	auto pixel_postprocess = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/PostProcess/PostProcessPixel.hlsl");
	auto pixel_pbr = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/PBR/PBRPixel.hlsl");
	auto pixel_shadow = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/Shadows/ShadowPixel.hlsl");
	auto pixel_depth_resolve_msaa = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/Particles/DepthBufferResolvePixel.hlsl");
	auto pixel_particle = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/Particles/ParticlePixel.hlsl");
	auto diffuse_irradiance_gen_pixel = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/IrradianceMaps/DiffuseIrradianceGenerator.hlsl");
	auto specular_irradiance_gen_pixel = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/IrradianceMaps/SpecularIrradianceGenerator.hlsl");
	auto specular_brdf_gen_pixel = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/IrradianceMaps/SpecularBRDFPrecompute.hlsl");
	auto dissolution_pbr_pixel = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/Dissolution/DissolutionPBRPixel.hlsl");
	auto geometry_shader_normals = shadersManager.AddGeometryShader(L"../../Engine/DXRenderer/Resources/Shaders/Normals/GeometryNormals.hlsl");
	auto geometry_shader_offset_wave = shadersManager.AddGeometryShader(L"../../Engine/DXRenderer/Resources/Shaders/Default/GeometryOffsetWave.hlsl");
	auto geometry_pass_through = shadersManager.AddGeometryShader(L"../../Engine/DXRenderer/Resources/Shaders/Default/GeometryPassThrough.hlsl");
	auto geometry_cube_shadow = shadersManager.AddGeometryShader(L"../../Engine/DXRenderer/Resources/Shaders/Shadows/CubeShadowGeometry.hlsl");
	auto hull_shader = shadersManager.AddHullShader(L"../../Engine/DXRenderer/Resources/Shaders/Default/Hull.hlsl");
	auto domain_shader = shadersManager.AddDomainShader(L"../../Engine/DXRenderer/Resources/Shaders/Default/Domain.hlsl");

	auto deferred_pbr = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/Deferred/PBRDeferred.hlsl");
	auto deferred_dissolution = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/Deferred/DissolutionDeferred.hlsl");
	auto deferred_emissive = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/Deferred/EmissiveDeferred.hlsl");
	auto deferred_resolve_pbr = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/Deferred/GBufferResolvePBR.hlsl");
	auto deferred_resolve_emissive = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/Deferred/GBufferResolveEmissive.hlsl");

	auto decal_vertex = shadersManager.AddVertexShader(L"../../Engine/DXRenderer/Resources/Shaders/Deferred/Decals/DecalVertex.hlsl", iedDecal);
	auto decal_pixel = shadersManager.AddPixelShader(L"../../Engine/DXRenderer/Resources/Shaders/Deferred/Decals/DecalPixel.hlsl");


	auto PBRShaders = std::make_shared<ShadersGroup>(ShadersGroup{ vertex_shader_instanced, deferred_pbr, nullptr, nullptr, nullptr });
	auto lightDefault = std::make_shared<ShadersGroup>(ShadersGroup{ vertex_shader_instanced, deferred_emissive, nullptr, nullptr, nullptr });
	auto dissolutionPBRShaders = std::make_shared<ShadersGroup>(ShadersGroup{ vertex_shader_instanced, deferred_dissolution, nullptr, nullptr, nullptr });

	auto defaultDepthStencilState = DepthStencilState(DepthStencilState::CreateDepthStencilStateDesc(true, true));
	auto defaultRasterizerState = RasterizerState(RasterizerState::CreateRasterizerStateDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, 0, 0, false));

	auto defaultPBRPipelineState = std::make_shared<PipelineState>(PipelineState{ defaultDepthStencilState, defaultRasterizerState, 1 });
	auto defaultLightSourcePipelineState = std::make_shared<PipelineState>(PipelineState{ defaultDepthStencilState, defaultRasterizerState, 2 });

	TransformDX cube1;
	cube1.position = { 0, 2, 2, 1 };
	cube1.rotation = DirectX::XMQuaternionRotationRollPitchYaw(0, 0, 0);
	cube1.scale = { 1, 1, 1, 1 };

	TransformDX lightSource1;
	lightSource1.position = { 0, 0, 0, 1 };
	lightSource1.rotation = DirectX::XMQuaternionRotationRollPitchYaw(0, 0, 0);
	lightSource1.scale = { 1, 1, 1, 1 };

	auto samurai = modelsManager.GetModel(L"Samurai/Samurai.fbx");
	samurai->SetDefaultShadersForAllMeshes(PBRShaders, OpacityMeshGroup::Opaque);
	samurai->SetDefaultShadersForAllMeshes(PBRShaders, OpacityMeshGroup::Translucent);
	samurai->SetDefaultPipelineStateForAllMeshes(defaultPBRPipelineState, OpacityMeshGroup::Opaque);
	samurai->SetDefaultPipelineStateForAllMeshes(defaultPBRPipelineState, OpacityMeshGroup::Translucent);
	samurai->InitOctree();

	auto cube = GetUnitCubeIndexed();
	auto instancedCubeMesh = std::make_shared<std::vector<InstancedMesh<Vertex>>>();
	instancedCubeMesh->emplace_back("unitCubeMesh", cube.vertices, cube.indices);
	instancedCubeMesh->at(0).SetBoundingBox(BoxDX({{-1, -1, -1, 1}, {1, 1, 1, 1}}));
	instancedCubeMesh->at(0).InitOctree();
	auto instancedCube = std::make_shared<Model>(instancedCubeMesh);
	modelsManager.AddModel(L"unitCube", instancedCube);

	auto sphere = GetUnitSphere();
	auto instancedSphereMesh = std::make_shared<std::vector<InstancedMesh<Vertex>>>();
	instancedSphereMesh->emplace_back(sphere);
	instancedSphereMesh->at(0).SetBoundingBox(BoxDX({ {-1, -1, -1, 1}, {1, 1, 1, 1} }));
	instancedSphereMesh->at(0).InitOctree();
	auto instancedSphere = std::make_shared<Model>(instancedSphereMesh);
	modelsManager.AddModel(L"unitSphere", instancedSphere);

	auto emptyMesh = std::make_shared<std::vector<InstancedMesh<Vertex>>>();
	emptyMesh->emplace_back("emptyMesh", std::vector<Vertex>{}, std::vector<uint32_t>{});
	auto emptyModel = std::make_shared<Model>(emptyMesh);
	modelsManager.AddModel(L"emptyModel", emptyModel);

	TransformDX tempTrans = cube1;
	
	auto tempMat = std::make_shared<Material>(Material::CreateMat(PBRShaders, DxResPtr<ID3D11Texture2D>{}, XMVECTOR{1, 1, 1, 1}, defaultPBRPipelineState));
	auto tempMat2 = std::make_shared<Material>(Material::CreateMat(PBRShaders, testTexture2, XMVECTOR{ 1, 1, 1, 1 }, defaultPBRPipelineState));
	tempMat->m_albedo = graniteColor;
	tempMat->m_normal = graniteNormal;
	tempMat->m_roughness = graniteRoughness;
	tempMat->m_metalness = graniteMetal;
	auto dissolutionMat = std::make_shared<Material>(Material::CreateMat(dissolutionPBRShaders, DxResPtr<ID3D11Texture2D>{}, XMVECTOR{ 1, 1, 1, 1 }, defaultPBRPipelineState));

	tempMat->m_pipelineState->stencilRefValue = 1;
	tempMat2->m_pipelineState->stencilRefValue = 1;
	dissolutionMat->m_pipelineState->stencilRefValue = 1;

	materialsManager.AddMaterial(L"granite", tempMat);
	materialsManager.AddMaterial(L"defaultPBRNoTex", tempMat2);
	materialsManager.AddMaterial(L"dissolutionPBR", dissolutionMat);

	for (int i = 0; i < 2; i++)
	{
		for (int k = 0; k < 2; k++)
		{
			tempTrans.position = DirectX::XMVectorAdd(cube1.position, DirectX::XMVectorSet(2 * i, -3, 2 * k, 0));
			tempTrans.scale = DirectX::XMVectorAdd(cube1.scale, DirectX::XMVectorSet(2 * i + k, 2 * i + k, 2 * i + k, 0));
			samurai->AddInstanceWithDefaultMaterial(tempTrans, OpacityMeshGroup::Opaque);
			for (int m = 0; m < 2; m++)
			{
				tempTrans.position = DirectX::XMVectorAdd(cube1.position + XMVectorSet(5, 1 * m, 5, 0), DirectX::XMVectorSet(10 + i * 5, 10 + k * 5, 10 + m * 5, 0));
				instancedCube->AddInstance(tempTrans, tempMat, OpacityMeshGroup::Opaque);
			}
		}
	}
	tempTrans.position += DirectX::XMVectorSet(10, 10, 10, 1);
	instancedCube->AddInstance(tempTrans, tempMat2, OpacityMeshGroup::Opaque);

	tempTrans.position = DirectX::XMVectorSet(0, -1.1, 0, 1);
	tempTrans.scale = DirectX::XMVectorSet(10, 0.1, 10, 1);
	instancedCube->AddInstance(tempTrans, tempMat, OpacityMeshGroup::Opaque);

	tempTrans.position = DirectX::XMVectorSet(0, 3, -5, 1);
	tempTrans.scale = DirectX::XMVectorSet(1, 1, 1, 1);
	auto& lightSystem = LightSystem::GetInstance();
	auto whiteMaterialOfLightMat = std::make_shared<Material>(Material::CreateMat(lightDefault, XMVectorSet(1, 1, 1, 1), XMVectorSet(50, 50, 50, 1), defaultLightSourcePipelineState));
	auto blueMaterialOfLightMat = std::make_shared<Material>(Material::CreateMat(lightDefault, XMVectorSet(0.5, 0.2, 1, 1), XMVectorSet(25, 10, 50, 1), defaultLightSourcePipelineState));
	auto orangeMaterialOfLightMat = std::make_shared<Material>(Material::CreateMat(lightDefault, XMVectorSet(1, 0.6, 0.3, 1), XMVectorSet(50, 30, 15, 1), defaultLightSourcePipelineState));
	
	materialsManager.AddMaterial(L"whiteMaterialOfLight", whiteMaterialOfLightMat);
	auto defaultLightMat = LightMaterial{ whiteMaterialOfLightMat, 1.0f };
	lightSystem.AddPointLight(instancedSphere, defaultLightMat, tempTrans, lightSource1);
	tempTrans.position.m128_f32[0] += 4;
	defaultLightMat.material = blueMaterialOfLightMat;
	lightSystem.AddPointLight(instancedSphere, defaultLightMat, tempTrans, lightSource1);
	tempTrans.position.m128_f32[0] += 4;
	defaultLightMat.material = orangeMaterialOfLightMat;
	//lightSystem.AddPointLight(instancedSphere, defaultLightMat, tempTrans, lightSource1);

	tempTrans = camera.m_transform;
	tempTrans.scale = XMVectorSet(0.1, 0.1, 0.1, 1);
	defaultLightMat.material = whiteMaterialOfLightMat;
	defaultLightMat.intensity = 1000.0f;
	whiteMaterialOfLightMat->m_albedo = spotlightMask;
	lightSystem.AddSpotLight(emptyModel, defaultLightMat, tempTrans, lightSource1, 0.2f, 0.05f);
	defaultLightMat.intensity = 1.0f;

	tempTrans.rotation = DirectX::XMQuaternionRotationRollPitchYaw(AI_MATH_PI * (0.15f), -AI_MATH_PI * (0.3f), 0);
	orangeMaterialOfLightMat->emissionColor /= 10;
	defaultLightMat.material = orangeMaterialOfLightMat;
	lightSystem.AddDirLight(tempTrans, defaultLightMat);

	tempTrans.position = { 2, -7, 0, 1 };
	tempTrans.scale = { 0.1, 0.1, 0.1, 1 };
	auto emitterModel = instancedSphere->AddInstance(tempTrans, blueMaterialOfLightMat, OpacityMeshGroup::Opaque);
	auto smokeEmitter = particleSystem.AddSmokeEmitter(0.35f, 1000, 0.1, emitterModel);
	smokeEmitter->SetTexturesAndColors({ 1, 1, 1, 1 }, { 5, 2, 10, 1 }, smokeEMVA, smokeDBF, smokeRLU);
	defaultLightMat.material = blueMaterialOfLightMat;
	lightSystem.AddPointLightToModel(emitterModel, defaultLightMat, TransformDX::IdentityTransform());

	tempTrans.position = { -6, -7, 0, 1 };
	auto emitterModel2 = instancedSphere->AddInstance(tempTrans, blueMaterialOfLightMat, OpacityMeshGroup::Opaque);
	auto smokeEmitter2 = particleSystem.AddSmokeEmitter(0.35f, 1000, 0.1, emitterModel2);
	smokeEmitter2->SetTexturesAndColors({ 1, 1, 1, 1 }, { 5, 2, 10, 1 }, smokeEMVA, smokeDBF, smokeRLU);
	defaultLightMat.material = blueMaterialOfLightMat;
	lightSystem.AddPointLightToModel(emitterModel2, defaultLightMat, TransformDX::IdentityTransform());
	
	auto& reflectionCapturer = ReflectionCapturer::GetInstance();
	reflectionCapturer.SetIrradianceTexturesCreationShaders(vertex_irradiance_gen, diffuse_irradiance_gen_pixel, specular_irradiance_gen_pixel, specular_brdf_gen_pixel);

	auto& renderer = Renderer::GetInstance();
	renderer.SetDebugShaders(vertex_pass_through_instanced, pixel_solid_color, hull_shader, domain_shader, geometry_shader_normals);
	renderer.InitSkybox(vertex_shader_skybox, pixel_skybox, L"grass_field.dds");
	renderer.SetNoiseMap(noiseTexture);
	
	auto& postProcess = PostProcess::GetInstance();
	postProcess.SetPostProcessShaders(vertex_postprocess, pixel_postprocess);
}







bool ApplicationDX::ProcessInput(const MSG& msg)
{
	static MeshIntersection capturedObject;
	static bool RMBpressed = false;
	static bool LMBpressed = false;
	static int currentMouseX;
	static int currentMouseY;
	static int lastMouseX;
	static int lastMouseY;

	static Engine& engine = Engine::GetInstance();
	static ModelsManager& modelsManager = ModelsManager::GetInstance();

	auto& renderer = Renderer::GetInstance();

	if (msg.message == WM_QUIT)
	{
		return false;
	}
	else if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
	{
		return true;
	}

	switch (msg.message)
	{
	case WM_MOUSEMOVE:
	{
		lastMouseX = currentMouseX;
		lastMouseY = currentMouseY;
		currentMouseX = LOWORD(msg.lParam);
		currentMouseY = HIWORD(msg.lParam);

		RECT rc;
		POINT pt = { currentMouseX , currentMouseY};
		GetClientRect(m_window.m_hWnd, &rc);
		if (!PtInRect(&rc, pt))
		{
			currentMouseX = lastMouseX;
			currentMouseY = lastMouseY;
		}

		engine.m_cursor.currentMouseX = currentMouseX;
		engine.m_cursor.currentMouseY = currentMouseY;
		engine.m_cursor.deltaX += currentMouseX - lastMouseX;
		engine.m_cursor.deltaY += currentMouseY - lastMouseY;
		
		if (engine.m_keys.LMB && capturedObject.instance != nullptr)
		{
			engine.DragObject(capturedObject, currentMouseX - lastMouseX, currentMouseY - lastMouseY);
		}
		break;
	}
	case WM_RBUTTONDOWN:
	{
		capturedObject.reset(0.01f);
		engine.m_keys.RMB = true;
		break;
	}
	case WM_LBUTTONDOWN:
		engine.m_keys.LMB = true;
		currentMouseX = LOWORD(msg.lParam);
		currentMouseY = HIWORD(msg.lParam);
		if (!engine.m_keys.RMB)
		{
			engine.CastRayToMove(currentMouseX, currentMouseY, capturedObject);
		}
		break;
	case WM_RBUTTONUP:
		engine.m_keys.RMB = false;
		break;
	case WM_LBUTTONUP:
		capturedObject.reset(0.01f);
		engine.m_keys.LMB = false;
		break;
	case WM_KEYDOWN:
		switch (msg.wParam)
		{
		case 0x57: //W
			engine.m_keys.W = true;
			break;
		case 0x41: //A
			engine.m_keys.A = true;
			break;
		case 0x53: //S
			engine.m_keys.S = true;
			break;
		case 0x44: //D
			engine.m_keys.D = true;
			break;
		case VK_SPACE:
			engine.m_keys.SPACE = true;
			break;
		case VK_CONTROL:
			engine.m_keys.CTRL = true;
			break;
		case 0x46:
			engine.m_keys.F = true;
			break;
		case 0x45: //E
			engine.m_keys.E = true;
			break;
		case 0x51: //Q
			engine.m_keys.Q = true;
			break;
		case 0x4E:
		{
			static bool debugFlag = false;
			debugFlag = !debugFlag;
			renderer.SetDebugFlag(debugFlag);
			break;
		}
		case 0x4D:
		{
			auto& camera = engine.GetCamera();
			TransformDX transform = camera.m_transform;
			transform.scale = XMVectorSet(1, 1, 1, 1);
			transform.position += 3 * camera.m_transform.Forward();
			transform.position -= camera.m_transform.Up();
			auto model = modelsManager.GetModel(L"Samurai/Samurai.fbx")->AddInstanceWithDefaultMaterial(transform, OpacityMeshGroup::Opaque);
			
			std::shared_ptr<Material> dissolutionPBR;
			std::shared_ptr<Material> defaultPBR;
			MaterialsManager::GetInstance().GetMaterial(L"dissolutionPBR", dissolutionPBR);
			MaterialsManager::GetInstance().GetMaterial(L"defaultPBRNoTex", defaultPBR);
			model->ChangeShaders(dissolutionPBR->shaders);
			
			engine.GetTimer().AddTimeTriggeredEvent(5.0f, [=] 
				{
					model->ChangeShaders(defaultPBR->shaders);
				});

			break;
		}
		}
		break;
	case WM_KEYUP:
		switch (msg.wParam)
		{
		case 0x57: //W
			engine.m_keys.W = false;
			break;
		case 0x41: //A
			engine.m_keys.A = false;
			break;
		case 0x53: //S
			engine.m_keys.S = false;
			break;
		case 0x44: //D
			engine.m_keys.D = false;
			break;
		case VK_SPACE:
			engine.m_keys.SPACE = false;
			break;
		case VK_CONTROL:
			engine.m_keys.CTRL = false;
			break;
		case 0x46:
			engine.m_keys.F = false;
			break;
		case 0x45: //E
			engine.m_keys.E = false;
			break;
		case 0x51: //Q
			engine.m_keys.Q = false;
			break;
		}
		break;
	}
	return true;
}

void ApplicationDX::Start()
{
	auto& engine = Engine::GetInstance();
	engine.Run();
}
