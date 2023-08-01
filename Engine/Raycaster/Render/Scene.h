#pragma once
#include <vector>
#include "../Utils/Camera.h"
#include "../Source/Math/Ray.h"
#include "../Source/Math/PointLight.h"
#include "../Source/Math/Sphere.h"
#include "../Source/Math/Triangle.h"
#include "../Source/Entities/PlaneEntity.h"
#include "../Source/Entities/TriangleEntity.h"
#include "../Source/Entities/SphereEntity.h"
#include "../Source/Entities/MeshEntity.h"
#include "../Source/Entities/DirLightEntity.h"
#include "../Source/Entities/PointLightEntity.h"
#include "../Source/Entities/SpotLightEntity.h"
#include "../Utils/TriangleOctree.h"
#include "../../Common/ParallelExecutor.h"
#include "../Source/Draggers/SphereMover.h"
#include "../Source/Draggers/PlaneMover.h"
#include "../Source/Draggers/TriangleMover.h"
#include "../Source/Draggers/MeshMover.h"

class Scene
{
public:
	Scene();
	~Scene();

	void Render(const int& width, const int& height, uint32_t* const& buffer, const Camera& camera);
	bool CheckWorldHitObjects(const Ray& ray, const Camera& camera, float maxT, ObjectRef& outRef, HitRecord& rec, std::shared_ptr<Material>& outMaterial);
	bool CheckWorldHitLights(const Ray& ray, const Camera& camera, float maxT, ObjectRef& outRef, HitRecord& rec, std::shared_ptr<LightMat>& outMaterial);
	void CalculatePixel(const Camera& camera, int width, int height, uint32_t x, uint32_t y, uint32_t* buffer, const glm::mat4& invMat);
	uint32_t GetColor(const Ray& ray, const Camera& camera);
	uint32_t GetLightedColor(const HitRecord& rec, std::shared_ptr<Material>& outMaterial, const Camera& camera);
	IObjectMover* CastRayToMove(const Camera& camera, HitRecord& rec, int width, int height, uint32_t x, uint32_t y);

	std::vector<SphereEntity> m_spheres;
	std::vector<TriangleEntity> m_triangles;
	std::vector<PlaneEntity> m_planes;
	std::vector<MeshEntity> m_meshes;

	std::vector<PointLightEntity> m_pointLights;
	std::vector<DirLightEntity> m_directLights;
	std::vector<SpotLightEntity> m_spotLights;
};