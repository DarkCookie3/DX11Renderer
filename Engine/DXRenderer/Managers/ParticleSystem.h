#pragma once
#include <vector>
#include "../CustomTypes/SmokeEmitter.h"

class ParticleSystem
{
public:
	static ParticleSystem& GetInstance()
	{
		static ParticleSystem instance;
		return instance;
	}
	ParticleSystem(const ParticleSystem&) = delete;
	ParticleSystem& operator=(const ParticleSystem&) = delete;

	std::shared_ptr<SmokeEmitter> AddSmokeEmitter(float spawnRate, uint32_t maxParticles, float spawnRadius, std::shared_ptr<Model> model);
	void Update(float deltaTime);
	void Render(DxResPtr<ID3D11Texture2D> depthStencilTexture);
private:
	std::vector<std::shared_ptr<SmokeEmitter>> m_smokeEmitters = {};

	ParticleSystem() {}
};