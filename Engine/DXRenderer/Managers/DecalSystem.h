#pragma once
#include "../CustomTypes/GBuffer.h"
#include "../CustomTypes/Decal.h"

class DecalSystem
{
public:
	static DecalSystem& GetInstance()
	{
		static DecalSystem instance;
		return instance;
	}
	DecalSystem(const DecalSystem&) = delete;
	DecalSystem& operator=(const DecalSystem&) = delete;

	void AddDecal(const Decal& decal);
	void Render();

private:
	DecalSystem() {}

	std::vector<Decal> m_decals = {};
};