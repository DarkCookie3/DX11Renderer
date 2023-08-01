#include "LightSystem.h"
#include "../../Common/Engine.h"
#include "MeshSystem.h"
#include "../Utils/CubeSideIndexRoationAngles.h"

struct ViewAndProjMatricesPair
{
	XMMATRIX view;
	XMMATRIX proj;
};

struct CubeViewAndProjMatrices
{
	XMMATRIX view[6];
	XMMATRIX proj;
};

XMMATRIX GenerateRotationMatrixByCubeSideIndex(int i)
{
	float yaw;
	float pitch;
	GetCubeSideCaptureAngles(yaw, pitch, i);

	return XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), 0);
}

std::vector<XMVECTOR> cameraFrustrumCorners(const XMMATRIX& view, const XMMATRIX& proj)
{
	std::vector<XMVECTOR> frustumCorners = {};

	const auto inv = XMMatrixInverse(0, XMMatrixMultiply(view, proj));

	for (unsigned int x = 0; x < 2; ++x)
	{
		for (unsigned int y = 0; y < 2; ++y)
		{
			for (unsigned int z = 0; z < 2; ++z)
			{
				XMVECTOR pt = XMVector4Transform(XMVectorSet(2.0f * x - 1.0f, 2.0f * y - 1.0f, z * 1.0f, 1.0f), inv);
				frustumCorners.push_back(pt / pt.m128_f32[3]);
			}
		}
	}
	return frustumCorners;
}

ViewAndProjMatricesPair GenerateDirLightShadowmapViewProjMatrix(PackedDirLight& light)
{
	auto& engine = Engine::GetInstance();
	auto& window = engine.GetWindow();
	auto& camera = engine.GetCamera();

	float aspect = float(window.m_windowW) / window.m_windowH;
	auto frustrumCorners = cameraFrustrumCorners(camera.generateViewMat(), camera.generateProjMat(aspect));

	auto center = XMVectorSet(0, 0, 0, 0);
	for (const auto& v : frustrumCorners)
	{
		center += v;
	}
	center /= frustrumCorners.size();

	auto lightView = XMMatrixLookToLH(
		center,
		light.direction,
		{ 0.0f, 1.0f, 0.0f, 0.0f }
	);

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::lowest();
	for (auto& v : frustrumCorners)
	{
		auto trf = XMVector4Transform(v, lightView);
		minX = std::min(minX, trf.m128_f32[0]);
		maxX = std::max(maxX, trf.m128_f32[0]);
		minY = std::min(minY, trf.m128_f32[1]);
		maxY = std::max(maxY, trf.m128_f32[1]);
		minZ = std::min(minZ, trf.m128_f32[2]);
		maxZ = std::max(maxZ, trf.m128_f32[2]);
	}

	minZ -= SHADOW_MARGIN;

	XMMATRIX lightProjection = XMMatrixOrthographicLH(maxX - minX, maxY - minY, maxZ, minZ);

	return { lightView, lightProjection };
}

ViewAndProjMatricesPair GenerateSpotLightShadowmapViewProjMatrix(PackedSpotLight& light)
{
	auto& engine = Engine::GetInstance();
	auto& window = engine.GetWindow();
	auto& camera = engine.GetCamera();

	auto lightView = light.worldToLocal;

	XMMATRIX lightProjection = XMMatrixPerspectiveFovLH(2 * (light.lightAngleRad + light.fadeAngleRad), 1.0f, camera.m_farPlaneDist, camera.m_nearPlaneDist);

	return { lightView, lightProjection };
}

CubeViewAndProjMatrices GeneratePointLightShadowmapViewProjMatrix(PackedPointLight& light)
{
	CubeViewAndProjMatrices result;

	auto& engine = Engine::GetInstance();
	auto& window = engine.GetWindow();
	auto& camera = engine.GetCamera();

	for (int i = 0; i < 6; i++)
	{
		auto RotateMatCubeSide = GenerateRotationMatrixByCubeSideIndex(i);
		RotateMatCubeSide.r[3].m128_f32[0] = light.position.m128_f32[0];
		RotateMatCubeSide.r[3].m128_f32[1] = light.position.m128_f32[1];
		RotateMatCubeSide.r[3].m128_f32[2] = light.position.m128_f32[2];
		result.view[i] = XMMatrixInverse(0, RotateMatCubeSide);
	}

	result.proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), 1.0f, camera.m_farPlaneDist, light.radius);

	return result;
}

void LightSystem::AddPointLight(std::shared_ptr<Model> model, LightMaterial lightMat, const TransformDX& modelTransform, const TransformDX& lightTransform)
{
	auto lightVisibleModel = model->AddInstance(modelTransform, lightMat.material, OpacityMeshGroup::Opaque);
	PointLight pl = PointLight{ lightTransform, lightVisibleModel, lightMat };
	m_pointLights.emplace_back(pl);
	pointLightSources.m_buffer = StructuredBuffer((void*)nullptr, sizeof(PackedPointLight), m_pointLights.size());
	pointLightSources.isArrangmentChanged = true;
}

void LightSystem::AddSpotLight(std::shared_ptr<Model> model, LightMaterial lightMat, const TransformDX& modelTransform, const TransformDX& lightTransform, float lightAngleRad, float fadeAngleRad)
{
	auto lightVisibleModel = model->AddInstance(modelTransform, lightMat.material, OpacityMeshGroup::Opaque);
	m_spotLights.emplace_back(SpotLight{ lightTransform, lightVisibleModel, lightAngleRad, fadeAngleRad, lightMat});
	spotLightSources.m_buffer = StructuredBuffer((void*)nullptr, sizeof(PackedSpotLight), m_spotLights.size());
	spotLightSources.isArrangmentChanged = true;
}

void LightSystem::AddDirLight(const TransformDX& lightTransform, LightMaterial lightMat)
{
	m_dirLights.emplace_back(DirLight{ lightTransform, lightMat });
	dirLightSources.m_buffer = StructuredBuffer((void*)nullptr, sizeof(PackedDirLight), m_dirLights.size());
	dirLightSources.isArrangmentChanged = true;
}

void LightSystem::AddPointLightToModel(std::shared_ptr<Model> model, LightMaterial lightMat, const TransformDX& lightTransform)
{
	PointLight pl = PointLight{ lightTransform, model, lightMat };
	m_pointLights.emplace_back(pl);
	pointLightSources.m_buffer = StructuredBuffer((void*)nullptr, sizeof(PackedPointLight), m_pointLights.size());
	pointLightSources.isArrangmentChanged = true;
}

void LightSystem::BindLightsSRV()
{
	auto& renderer = Renderer::GetInstance();
	auto& engine = Engine::GetInstance();
	auto& window = engine.GetWindow();
	auto& camera = engine.GetCamera();

	int initHeight = window.m_windowH;
	int initWidth = window.m_windowW;

	auto ConstBufferPerViewInit = *renderer.GetConstBufferData<CONSTANT_BUFFER_PER_VIEW_STRUCT>(BufferSlots::PER_VIEW);
	auto ConstantBufferPerFrameInit = *renderer.GetConstBufferData<CONSTANT_BUFFER_PER_FRAME_STRUCT>(BufferSlots::PER_FRAME);

	if (!m_dirLights.size() && !m_spotLights.size() && !m_pointLights.size())
	{
		return;
	}
	auto packedPointLights = GetPackedPointLights();
	auto packedSpotLights = GetPackedSpotLights(m_spotLightTextureMasks);
	auto packedDirLights = GetPackedDirLights();

	GenerateShadowMaps(packedDirLights, packedPointLights, packedSpotLights);
	window.ResizeViewport(initWidth, initHeight);

	if (packedPointLights.size() > 0)
	{
		pointLightSources.m_buffer.Update(packedPointLights.data());
		pointLightSources.m_buffer.Bind(8);
	}
	if (packedSpotLights.size() > 0)
	{
		spotLightSources.m_buffer.Update(packedSpotLights.data());
		spotLightSources.m_buffer.Bind(9);
	}
	if (packedDirLights.size() > 0)
	{
		dirLightSources.m_buffer.Update(packedDirLights.data());
		dirLightSources.m_buffer.Bind(10);
	}

	m_spotLightTextureMasks.Bind(11);

	m_dirShadowMaps.Bind(15);
	m_pointShadowMaps.Bind(16);
	m_spotShadowMaps.Bind(17);

	auto ConstBufferPerView = renderer.GetConstBufferData<CONSTANT_BUFFER_PER_VIEW_STRUCT>(BufferSlots::PER_VIEW);
	auto ConstantBufferPerFrame = renderer.GetConstBufferData<CONSTANT_BUFFER_PER_FRAME_STRUCT>(BufferSlots::PER_FRAME);

	*ConstantBufferPerFrame = ConstantBufferPerFrameInit;
	*ConstBufferPerView = ConstBufferPerViewInit;

	ConstantBufferPerFrame->pointLightNum = packedPointLights.size();
	ConstantBufferPerFrame->spotLightNum = packedSpotLights.size();
	ConstantBufferPerFrame->dirLightNum = packedDirLights.size();

	renderer.UpdateConstBufferAtSlot<CONSTANT_BUFFER_PER_VIEW_STRUCT>(BufferSlots::PER_VIEW);
	renderer.UpdateConstBufferAtSlot<CONSTANT_BUFFER_PER_FRAME_STRUCT>(BufferSlots::PER_FRAME);
}

void LightSystem::GenerateShadowMaps(std::vector<PackedDirLight>& packedDirLights, std::vector<PackedPointLight>& packedPointLights, std::vector<PackedSpotLight>& packedSpotLights)
{
	auto& shadersManager = ShadersManager::GetInstance();

	DxResPtr<ID3D11RasterizerState> shadowRasterizerState;
	DxResPtr<ID3D11DepthStencilState> depthStencilState;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

	HRESULT H = s_device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.reset());
	s_devcon->OMSetDepthStencilState(depthStencilState, 0);

	D3D11_RASTERIZER_DESC shadowRasterizerDesc;
	shadowRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	shadowRasterizerDesc.CullMode = D3D11_CULL_BACK;
	shadowRasterizerDesc.FrontCounterClockwise = false;
	shadowRasterizerDesc.DepthBias = -3;
	shadowRasterizerDesc.DepthBiasClamp = 0;
	shadowRasterizerDesc.SlopeScaledDepthBias = -2;
	shadowRasterizerDesc.DepthClipEnable = true;
	shadowRasterizerDesc.ScissorEnable = false;
	shadowRasterizerDesc.MultisampleEnable = false;
	shadowRasterizerDesc.AntialiasedLineEnable = false;

	H = s_device->CreateRasterizerState(&shadowRasterizerDesc, shadowRasterizerState.reset());
	s_devcon->RSSetState(shadowRasterizerState);

	s_devcon->HSSetShader(nullptr, nullptr, 0);
	s_devcon->DSSetShader(nullptr, nullptr, 0);
	s_devcon->GSSetShader(nullptr, nullptr, 0);

	shadersManager.GetVertexShader(L"FlatShadowVertex")->Bind();
	shadersManager.GetPixelShader(L"ShadowPixel")->Bind();
	{
		GenerateDirShadowMaps(packedDirLights);
		GenerateSpotShadowMaps(packedSpotLights);
	}
	
	shadersManager.GetGeometryShader(L"CubeShadowGeometry")->Bind();
	shadersManager.GetVertexShader(L"ShadowVertexPassThrough")->Bind();
	{
		GeneratePointShadowMaps(packedPointLights);
	}
	
	ID3D11RenderTargetView* nullRTV = nullptr;
	s_devcon->OMSetRenderTargets(1, &nullRTV, nullptr);
}

void LightSystem::GenerateDirShadowMaps(std::vector<PackedDirLight>& packedDirLights)
{
	if (packedDirLights.size() == 0) { return; }

	DxResPtr<ID3D11Texture2D> shadowMapsTextures;
	DxResPtr<ID3D11DepthStencilView> shadowDepthStencilView;

	auto& engine = Engine::GetInstance();
	auto& renderer = Renderer::GetInstance();
	auto& meshSystem = MeshSystem::GetInstance();
	auto& window = engine.GetWindow();
	auto camera = engine.GetCamera();

	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	shadowMapDesc.Width = 2048;
	shadowMapDesc.Height = 2048;
	shadowMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowMapDesc.ArraySize = packedDirLights.size();
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.CPUAccessFlags = 0;
	shadowMapDesc.MiscFlags = 0;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.SampleDesc.Quality = 0;
	shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;

	s_device->CreateTexture2D(&shadowMapDesc, nullptr, shadowMapsTextures.reset());

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depthStencilViewDesc.Texture2DArray.ArraySize = -1;
	depthStencilViewDesc.Texture2DArray.MipSlice = 0;

	window.ResizeViewport(shadowMapDesc.Width, shadowMapDesc.Height);

	int lightIndex = 0;
	for (auto& light : packedDirLights)
	{
		depthStencilViewDesc.Texture2DArray.FirstArraySlice = lightIndex;
		s_device->CreateDepthStencilView(shadowMapsTextures, &depthStencilViewDesc, shadowDepthStencilView.reset());
		s_devcon->OMSetRenderTargets(0, nullptr, shadowDepthStencilView);
		s_devcon->ClearDepthStencilView(shadowDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

		auto lightViewProjPair = GenerateDirLightShadowmapViewProjMatrix(light);

		float viewSizeX = 2.0 / (shadowMapDesc.Width * lightViewProjPair.proj.r[0].m128_f32[0]);
		float viewSizeY = 2.0 / (shadowMapDesc.Height * lightViewProjPair.proj.r[1].m128_f32[1]);
		lightViewProjPair.view.r[3].m128_f32[0] = round(lightViewProjPair.view.r[3].m128_f32[0] / viewSizeX) * viewSizeX;
		lightViewProjPair.view.r[3].m128_f32[1] = round(lightViewProjPair.view.r[3].m128_f32[1] / viewSizeY) * viewSizeY;
		
		auto lightViewProj = lightViewProjPair.view * lightViewProjPair.proj;

		auto ConstDataPerView = renderer.GetConstBufferData<CONSTANT_BUFFER_PER_VIEW_STRUCT>(BufferSlots::PER_VIEW);
		ConstDataPerView->viewProj = lightViewProj;
		renderer.UpdateConstBufferAtSlot<CONSTANT_BUFFER_PER_VIEW_STRUCT>(BufferSlots::PER_VIEW);

		meshSystem.Render(OpacityMeshGroup::Opaque, true, true, true);

		light.shadowMapIndex = lightIndex;
		light.lightViewProj = lightViewProj;
		lightIndex++;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize = shadowMapDesc.ArraySize;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.MipLevels = shadowMapDesc.MipLevels;
	srvDesc.Texture2DArray.MostDetailedMip = 0;

	DxResPtr<ID3D11Resource> shadowMapResource;
	shadowMapsTextures->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(shadowMapResource.reset()));

	m_dirShadowMaps = ShaderResourceView(srvDesc, shadowMapResource);
}

void LightSystem::GeneratePointShadowMaps(std::vector<PackedPointLight>& packedPointLights)
{
	if (packedPointLights.size() == 0) { return; }

	DxResPtr<ID3D11Texture2D> shadowMapsTextures;
	DxResPtr<ID3D11DepthStencilView> shadowDepthStencilView;

	auto& engine = Engine::GetInstance();
	auto& renderer = Renderer::GetInstance();
	auto& meshSystem = MeshSystem::GetInstance();
	auto& window = engine.GetWindow();
	auto camera = engine.GetCamera();

	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	shadowMapDesc.Width = 1024;
	shadowMapDesc.Height = 1024;
	shadowMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowMapDesc.ArraySize = 6 * packedPointLights.size();
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.CPUAccessFlags = 0;
	shadowMapDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.SampleDesc.Quality = 0;
	shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;

	s_device->CreateTexture2D(&shadowMapDesc, nullptr, shadowMapsTextures.reset());

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depthStencilViewDesc.Texture2DArray.ArraySize = -1;
	depthStencilViewDesc.Texture2DArray.MipSlice = 0;

	window.ResizeViewport(shadowMapDesc.Width, shadowMapDesc.Height);

	int lightIndex = 0;
	for (auto& light : packedPointLights)
	{
		depthStencilViewDesc.Texture2DArray.FirstArraySlice = lightIndex * 6;
		s_device->CreateDepthStencilView(shadowMapsTextures, &depthStencilViewDesc, shadowDepthStencilView.reset());
		s_devcon->OMSetRenderTargets(0, nullptr, shadowDepthStencilView);
		s_devcon->ClearDepthStencilView(shadowDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

		auto lightViewProjMatrices = GeneratePointLightShadowmapViewProjMatrix(light);

		auto ConstDataPerView = renderer.GetConstBufferData<CONSTANT_BUFFER_ADDITIONAL_PER_VIEW_STRUCT>(BufferSlots::ADDITIONAL_PER_VIEW);
		for (int i = 0; i < 6; i++)
		{
			ConstDataPerView->viewProjCubeSides[i] = lightViewProjMatrices.view[i] * lightViewProjMatrices.proj;
		}
		renderer.UpdateConstBufferAtSlot<CONSTANT_BUFFER_ADDITIONAL_PER_VIEW_STRUCT>(BufferSlots::ADDITIONAL_PER_VIEW);

		meshSystem.Render(OpacityMeshGroup::Opaque, true, true, true);

		light.shadowMapIndex = lightIndex;
		for (int i = 0; i < 6; i++)
		{
			light.lightViewProj[i] = ConstDataPerView->viewProjCubeSides[i];
		}
		lightIndex++;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
	srvDesc.TextureCubeArray.First2DArrayFace = 0;
	srvDesc.TextureCubeArray.MostDetailedMip = 0;
	srvDesc.TextureCubeArray.MipLevels = 1;
	srvDesc.TextureCubeArray.NumCubes = packedPointLights.size();

	DxResPtr<ID3D11Resource> shadowMapResource;
	shadowMapsTextures->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(shadowMapResource.reset()));

	m_pointShadowMaps = ShaderResourceView(srvDesc, shadowMapResource);
}

void LightSystem::GenerateSpotShadowMaps(std::vector<PackedSpotLight>& packedSpotLights)
{
	if (packedSpotLights.size() == 0) { return; }

	DxResPtr<ID3D11Texture2D> shadowMapsTextures;
	DxResPtr<ID3D11DepthStencilView> shadowDepthStencilView;

	auto& engine = Engine::GetInstance();
	auto& renderer = Renderer::GetInstance();
	auto& meshSystem = MeshSystem::GetInstance();
	auto& window = engine.GetWindow();
	auto camera = engine.GetCamera();

	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	shadowMapDesc.Width = 1024;
	shadowMapDesc.Height = 1024;
	shadowMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowMapDesc.ArraySize = packedSpotLights.size();
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.CPUAccessFlags = 0;
	shadowMapDesc.MiscFlags = 0;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.SampleDesc.Quality = 0;
	shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;

	s_device->CreateTexture2D(&shadowMapDesc, nullptr, shadowMapsTextures.reset());

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depthStencilViewDesc.Texture2DArray.ArraySize = -1;
	depthStencilViewDesc.Texture2DArray.MipSlice = 0;

	window.ResizeViewport(shadowMapDesc.Width, shadowMapDesc.Height);

	int lightIndex = 0;
	for (auto& light : packedSpotLights)
	{
		depthStencilViewDesc.Texture2DArray.FirstArraySlice = lightIndex;
		s_device->CreateDepthStencilView(shadowMapsTextures, &depthStencilViewDesc, shadowDepthStencilView.reset());
		s_devcon->OMSetRenderTargets(0, nullptr, shadowDepthStencilView);
		s_devcon->ClearDepthStencilView(shadowDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
		
		auto lightViewProjPair = GenerateSpotLightShadowmapViewProjMatrix(light);

		auto lightViewProj = lightViewProjPair.view * lightViewProjPair.proj;

		auto ConstDataPerView = renderer.GetConstBufferData<CONSTANT_BUFFER_PER_VIEW_STRUCT>(BufferSlots::PER_VIEW);
		ConstDataPerView->viewProj = lightViewProj;
		renderer.UpdateConstBufferAtSlot<CONSTANT_BUFFER_PER_VIEW_STRUCT>(BufferSlots::PER_VIEW);

		meshSystem.Render(OpacityMeshGroup::Opaque, true, true, true);
		
		light.lightViewProj = lightViewProj;
		light.shadowMapIndex = lightIndex;
		lightIndex++;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize = shadowMapDesc.ArraySize;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.MipLevels = shadowMapDesc.MipLevels;
	srvDesc.Texture2DArray.MostDetailedMip = 0;

	DxResPtr<ID3D11Resource> shadowMapResource;
	shadowMapsTextures->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(shadowMapResource.reset()));

	m_spotShadowMaps = ShaderResourceView(srvDesc, shadowMapResource);
}

std::vector<PackedPointLight> LightSystem::GetPackedPointLights()
{
	std::vector<PackedPointLight> result;
	for (auto& source : m_pointLights)
	{
		result.push_back(source.GetPacked());
	}
	pointLightSources.isArrangmentChanged = false;
	return result;
}

std::vector<PackedDirLight> LightSystem::GetPackedDirLights()
{
	std::vector<PackedDirLight> result;
	for (auto& source : m_dirLights)
	{
		result.push_back(source.GetPacked());
	}
	dirLightSources.isArrangmentChanged = false;
	return result;
}

std::vector<PackedSpotLight> LightSystem::GetPackedSpotLights(ShaderResourceView& srv_out)
{
	std::vector<PackedSpotLight> result;
	std::vector<DxResPtr<ID3D11Texture2D>> texturesPacked;
	static std::vector<D3D11_TEXTURE2D_DESC> texturesDescsPacked;
	static UINT maxWidth = 0;
	static UINT maxHeight = 0;

	if (spotLightSources.isArrangmentChanged)
	{
		texturesDescsPacked.clear();
	}

	int sliceIndex = 0;
	for (auto& source : m_spotLights)
	{
		auto packedSourceLight = source.GetPacked();
		if (source.lightMat.material->m_albedo != nullptr)
		{
			packedSourceLight.texArraySliceIndex = sliceIndex++;
			if (spotLightSources.isArrangmentChanged)
			{
				D3D11_TEXTURE2D_DESC desc;
				source.lightMat.material->m_albedo->GetDesc(&desc);
				texturesDescsPacked.push_back(desc);

				texturesPacked.push_back(source.lightMat.material->m_albedo);
			}
		}
		else
		{
			packedSourceLight.texArraySliceIndex = -1;
		}
		result.push_back(packedSourceLight);
	}

	

	if (texturesPacked.size() > 0 && spotLightSources.isArrangmentChanged)
	{
		for (auto& desc : texturesDescsPacked)
		{
			if (desc.Width > maxWidth)
				maxWidth = desc.Width;

			if (desc.Height > maxHeight)
				maxHeight = desc.Height;
		}

		D3D11_TEXTURE2D_DESC texArrayDesc;
		texturesPacked[0]->GetDesc(&texArrayDesc);
		texArrayDesc.Height = maxHeight;
		texArrayDesc.Width = maxWidth;
		texArrayDesc.ArraySize = texturesPacked.size();

		DxResPtr<ID3D11Texture2D> texArray;
		s_device->CreateTexture2D(&texArrayDesc, nullptr, texArray.reset());

		for (int i = 0; i < texturesPacked.size(); i++)
		{
			for (int k = 0; k < texArrayDesc.MipLevels; k++)
			{
				s_devcon->CopySubresourceRegion(texArray.ptr(), texArrayDesc.MipLevels * i + k, 0, 0, 0, texturesPacked[i], k, NULL);
			}
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = texArrayDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.ArraySize = texturesPacked.size();

		DxResPtr<ID3D11Resource> resource;
		texArray->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(resource.reset()));

		srv_out = ShaderResourceView(srvDesc, resource);
	}

	for (auto& instanceData : result)
	{
		if (instanceData.texArraySliceIndex >= 0)
		{
			auto& desc = texturesDescsPacked[instanceData.texArraySliceIndex];
			instanceData.texCoordScale[0] = desc.Width / float(maxWidth);
			instanceData.texCoordScale[1] = desc.Height / float(maxHeight);
		}
	}

	spotLightSources.isArrangmentChanged = false;
	return result;
}

