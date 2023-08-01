#pragma once
#include "../Managers/TransformSystem.h"
#include "SmokeParticle.h"
#include "InstancedMesh.h"

class SmokeEmitter
{
public:
	SmokeEmitter() = default;
	SmokeEmitter(float spawnRate, uint32_t maxParticles, float spawnRadius, std::shared_ptr<Model> model);
	
	void SetTexturesAndColors(const XMVECTOR& particleBaseColor, const XMVECTOR& particleEmissionColor, DxResPtr<ID3D11Texture2D> MVEA, DxResPtr<ID3D11Texture2D> lightmapDBF, DxResPtr<ID3D11Texture2D> lightmapRLU);

	void Update(float deltaTime);
	void Render(DxResPtr<ID3D11Texture2D> depthStencilTexture);
private:
	SmokeParticle SpawnParticle();
	void SortParticles();

	float m_spawnRate = 0.0f;
	float m_spawnRadius = 1.0f;
	uint32_t m_maxParticles = 0;
	XMVECTOR m_particleBaseColor;
	XMVECTOR m_particleEmissionColor;
	TransformDX_ID m_modelTrans;
	std::vector<uint32_t> m_particlesIndicesSorted = {};
	std::vector<SmokeParticle> m_particles = {};

	IndexBuffer m_indexBuffer = IndexBuffer({0, 1, 2, 3, 2, 1});
	InstanceBuffer m_instanceBuffer;
	ShaderResourceView m_smokeEMVA;
	ShaderResourceView m_smokeLightmapDBF;
	ShaderResourceView m_smokeLightmapRLU;
};