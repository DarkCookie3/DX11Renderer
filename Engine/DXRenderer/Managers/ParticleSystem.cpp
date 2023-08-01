#include "ParticleSystem.h"

std::shared_ptr<SmokeEmitter> ParticleSystem::AddSmokeEmitter(float spawnRate, uint32_t maxParticles, float spawnRadius, std::shared_ptr<Model> model)
{
	return m_smokeEmitters.emplace_back(std::make_shared<SmokeEmitter>(spawnRate, maxParticles, spawnRadius, model));
}

void ParticleSystem::Update(float deltaTime)
{
	for (auto& emitter : m_smokeEmitters)
	{
		emitter->Update(deltaTime);
	}
}

void ParticleSystem::Render(DxResPtr<ID3D11Texture2D> depthStencilTexture)
{
	for (auto& emitter : m_smokeEmitters)
	{
		emitter->Render(depthStencilTexture);
	}
}
