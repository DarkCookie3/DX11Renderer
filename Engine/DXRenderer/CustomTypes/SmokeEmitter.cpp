#include "SmokeEmitter.h"
#include "../../Common/Engine.h"
#include "../Utils/RandomOnSphere.h"


SmokeEmitter::SmokeEmitter(float spawnRate, uint32_t maxParticles, float spawnRadius, std::shared_ptr<Model> model) 
	: m_spawnRate(spawnRate), m_maxParticles(maxParticles), m_spawnRadius(spawnRadius), m_modelTrans(model->GetTransformID()),
	  m_particles(maxParticles), m_particlesIndicesSorted(maxParticles)
{
	for (int i = 0; i < maxParticles; i++)
	{
		auto& particle = m_particles.at(i);
		particle = SpawnParticle();
		particle.timeAlive = i / (maxParticles * spawnRate);
		m_particlesIndicesSorted.at(i) = i;
	}

	m_instanceBuffer.m_size = sizeof(PackedSmokeParticle) * m_maxParticles;
	m_instanceBuffer.m_buffer = VertexBuffer((void*)nullptr, m_instanceBuffer.m_size, false);
}

void SmokeEmitter::SetTexturesAndColors(const XMVECTOR& particleBaseColor, const XMVECTOR& particleEmissionColor, DxResPtr<ID3D11Texture2D> MVEA, DxResPtr<ID3D11Texture2D> lightmapDBF, DxResPtr<ID3D11Texture2D> lightmapRLU)
{
	m_particleBaseColor = particleBaseColor;
	m_particleEmissionColor = particleEmissionColor;
	for (int i = 0; i < m_maxParticles; i++)
	{
		m_particles.at(i).colorRGBA = m_particleBaseColor;
		m_particles.at(i).emissionColor = m_particleEmissionColor;
	}

	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	MVEA->GetDesc(&texDesc);

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	DxResPtr<ID3D11Resource> motionVectorsResource;
	MVEA->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(motionVectorsResource.reset()));

	m_smokeEMVA = ShaderResourceView(srvDesc, motionVectorsResource);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	lightmapDBF->GetDesc(&texDesc);

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	DxResPtr<ID3D11Resource> lightmapDBFResource;
	lightmapDBF->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(lightmapDBFResource.reset()));

	m_smokeLightmapDBF = ShaderResourceView(srvDesc, lightmapDBFResource);

	lightmapRLU->GetDesc(&texDesc);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	DxResPtr<ID3D11Resource> lightmapRLUResource;
	lightmapRLU->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(lightmapRLUResource.reset()));

	m_smokeLightmapRLU = ShaderResourceView(srvDesc, lightmapRLUResource);
}

void SmokeEmitter::Update(float deltaTime)
{
	static float maxAliveTime = 1.0f / m_spawnRate;
	static float sizeDelta = (maxSmokeParticleSize - minSmokeParticleSize) / maxAliveTime;
	static float fullyOpaqueTime = 0.2 * maxAliveTime;

	auto spawnCenter = m_modelTrans.GetTransform();

	for (auto& particle : m_particles)
	{
		if (particle.timeAlive >= maxAliveTime)
		{
			float newTimeAlive = fmod(particle.timeAlive, maxAliveTime);
			particle = SpawnParticle();
			particle.timeAlive = newTimeAlive;
			continue;
		}
		particle.colorRGBA.m128_f32[3] = (particle.timeAlive <= fullyOpaqueTime) ? (particle.timeAlive / (fullyOpaqueTime)) : ((maxAliveTime - particle.timeAlive) / (maxAliveTime - fullyOpaqueTime));
		particle.position += particle.velocity * deltaTime;
		particle.velocity += XMVectorSet(0, smokeParticleVerticalAcceleration, 0, 0) * deltaTime;
		particle.timeAlive += deltaTime;
		particle.animationState = fmod(smokeParticleAnimationRate * particle.timeAlive / maxAliveTime, 1.0f);
		particle.size[0] += sizeDelta * deltaTime;
		particle.size[1] += sizeDelta * deltaTime;
	}
	SortParticles();
}

void SmokeEmitter::Render(DxResPtr<ID3D11Texture2D> depthStencilTexture)
{
	auto& engine = Engine::GetInstance();
	auto& window = engine.GetWindow();
	auto& shadersManager = ShadersManager::GetInstance();
	auto postProcessVertex = shadersManager.GetVertexShader(L"PostProcessVertex");
	auto depthResolvePixel = shadersManager.GetPixelShader(L"DepthBufferResolvePixel");
	auto particleVertex = shadersManager.GetVertexShader(L"ParticleVertex");
	auto particlePixel = shadersManager.GetPixelShader(L"ParticlePixel");

	s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	s_devcon->HSSetShader(nullptr, nullptr, 0);
	s_devcon->DSSetShader(nullptr, nullptr, 0);
	s_devcon->GSSetShader(nullptr, nullptr, 0);

	postProcessVertex->Bind();
	depthResolvePixel->Bind();

	D3D11_TEXTURE2D_DESC texDesc;
	depthStencilTexture->GetDesc(&texDesc);

	DxResPtr<ID3D11RenderTargetView> newDepthRTV;
	DxResPtr<ID3D11Texture2D> newDepthTexture;
	D3D11_TEXTURE2D_DESC newDepthTextureDesc = texDesc;
	newDepthTextureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	newDepthTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	newDepthTextureDesc.SampleDesc.Count = 1;
	newDepthTextureDesc.SampleDesc.Quality = 0;

	s_device->CreateTexture2D(&newDepthTextureDesc, NULL, newDepthTexture.reset());
	s_device->CreateRenderTargetView(newDepthTexture, NULL, newDepthRTV.reset());
	auto const newDepthRTVPointer = newDepthRTV.ptr();
	s_devcon->OMSetRenderTargets(1, &newDepthRTVPointer, nullptr);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	DxResPtr<ID3D11Resource> depthStencilResource;
	depthStencilTexture->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(depthStencilResource.reset()));

	auto depthStencilOldSRV = ShaderResourceView(srvDesc, depthStencilResource);
	depthStencilOldSRV.Bind(0);

	s_devcon->Draw(3, 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvResolvedDepthDesc;
	srvResolvedDepthDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvResolvedDepthDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvResolvedDepthDesc.Texture2D.MipLevels = 1;
	srvResolvedDepthDesc.Texture2D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* pSRV = nullptr;
	s_devcon->VSSetShaderResources(0, 1, &pSRV);
	s_devcon->HSSetShaderResources(0, 1, &pSRV);
	s_devcon->DSSetShaderResources(0, 1, &pSRV);
	s_devcon->GSSetShaderResources(0, 1, &pSRV);
	s_devcon->PSSetShaderResources(0, 1, &pSRV);
	window.m_backbufferRenderTargetHDR.Bind();

	DxResPtr<ID3D11Resource> resolvedDepthResource;
	newDepthTexture->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<void**>(resolvedDepthResource.reset()));
	auto resolvedDepth_SRV = ShaderResourceView(srvResolvedDepthDesc, resolvedDepthResource);
	resolvedDepth_SRV.Bind(3);

	particleVertex->Bind();
	particlePixel->Bind();

	m_indexBuffer.Bind();
	m_smokeEMVA.Bind(0);
	m_smokeLightmapDBF.Bind(1);
	m_smokeLightmapRLU.Bind(2);

	std::vector<PackedSmokeParticle> packedParticles(m_maxParticles);
	for (int i = 0; i < m_maxParticles; i++)
	{
		packedParticles.at(i) = m_particles.at(m_particlesIndicesSorted.at(i)).GetPacked();
	}
	m_instanceBuffer.m_buffer.Update(packedParticles.data(), m_instanceBuffer.m_size);
	m_instanceBuffer.m_buffer.Bind(1, sizeof(PackedSmokeParticle), 0);

	s_devcon->DrawIndexedInstanced(6, m_particles.size(), 0, 0, 0);
}

SmokeParticle SmokeEmitter::SpawnParticle()
{
	SmokeParticle result;
	result.colorRGBA = m_particleBaseColor;
	result.emissionColor = m_particleEmissionColor;
	XMVECTOR direction = RandomOnUnitSphere();
	result.position = m_modelTrans.GetTransform().position + m_spawnRadius * direction;
	result.velocity = direction;
	result.rotation = XMConvertToRadians(float(rand() % 360));
	result.size = { minSmokeParticleSize, minSmokeParticleSize };
	return result;
}

void SmokeEmitter::SortParticles()
{
	auto cam_pos = Engine::GetInstance().GetCamera().m_transform.position;
	auto cam_dir = Engine::GetInstance().GetCamera().m_transform.Forward();
	auto comparisonPredicate = [this, cam_pos, cam_dir](uint32_t firstParticle, uint32_t secondParticle) 
	{
		return sqrt(abs(XMVector3Dot((m_particles[firstParticle].position - cam_pos), cam_dir).m128_f32[0])) > sqrt(abs(XMVector3Dot((m_particles[secondParticle].position - cam_pos), cam_dir).m128_f32[0]));
	};
	std::sort(m_particlesIndicesSorted.begin(), m_particlesIndicesSorted.end(), comparisonPredicate);
}
