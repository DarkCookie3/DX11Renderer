#include <algorithm>
#include <initializer_list>
#include "../../Submodules/glm/glm/glm.hpp"
#include "../../Submodules/glm/glm/gtc/matrix_transform.hpp"
#include "Scene.h"
#include "../Utils/HitRecord.h"
#include "../Utils/Material.h"
#include "../Utils/ObjectRef.h"


Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Render(const int& width, const int& height, uint32_t* const& buffer, const Camera& camera)
{
    ParallelExecutor parallelExecutor(8);

    auto view = camera.generateViewMat();
    auto proj = camera.generateProjMat(float(width)/height);

    auto invMat = glm::inverse(proj * view);

    auto func = [&](uint32_t threadIndex, uint32_t taskIndex)
    {
        CalculatePixel(camera, width, height, taskIndex % width, taskIndex / width, buffer, invMat);
    };

    parallelExecutor.execute(func, width*height, 200);
}

bool Scene::CheckWorldHitObjects(const Ray& ray, const Camera& camera, float maxT, ObjectRef& outRef, HitRecord& rec, std::shared_ptr<Material>& outMaterial)
{
    float far = maxT;
    bool hitAnything = false;
    HitRecord tempHitRecord;
    ObjectRef tempObjectRef;
    std::shared_ptr<Material> tempMaterial;

    for (auto& object : m_spheres)
    {
        if (object.Hit(ray, far, tempObjectRef, tempHitRecord, tempMaterial))
        {
            far = tempHitRecord.m_t;
            hitAnything = true;
            rec = tempHitRecord;
            outRef = tempObjectRef;
            outMaterial = tempMaterial;
        }
    } 
    for (auto& object : m_planes)
    {
        if (object.Hit(ray, far, tempObjectRef, tempHitRecord, tempMaterial))
        {
            far = tempHitRecord.m_t;
            hitAnything = true;
            rec = tempHitRecord;
            outRef = tempObjectRef;
            outMaterial = tempMaterial;
        }
    }
    
    for (auto& object : m_triangles)
    {
        if (object.Hit(ray, far, tempObjectRef, tempHitRecord, tempMaterial))
        {
            far = tempHitRecord.m_t;
            hitAnything = true;
            rec = tempHitRecord;
            outRef = tempObjectRef;
            outMaterial = tempMaterial;
        }
    }
    for (auto& object : m_meshes)
    {
        if (object.Hit(ray, far, tempObjectRef, tempHitRecord, tempMaterial))
        {
            far = tempHitRecord.m_t;
            hitAnything = true;
            rec = tempHitRecord;
            outRef = tempObjectRef;
            outMaterial = tempMaterial;
        }
    }
    
    return hitAnything;
}

bool Scene::CheckWorldHitLights(const Ray& ray, const Camera& camera, float maxT, ObjectRef& outRef, HitRecord& rec, std::shared_ptr<LightMat>& outMaterial)
{
    float far = maxT;
    bool hitAnything = false;
    HitRecord tempHitRecord;
    ObjectRef tempObjectRef;
    std::shared_ptr<LightMat> tempMaterial;

    for (auto& object : m_pointLights)
    {
        if (object.Hit(ray, far, tempObjectRef, tempHitRecord, tempMaterial))
        {
            far = tempHitRecord.m_t;
            hitAnything = true;
            rec = tempHitRecord;
            outRef = tempObjectRef;
            outMaterial = tempMaterial;
        }
    }

    for (auto& object : m_spotLights)
    {
        if (object.Hit(ray, far, tempObjectRef, tempHitRecord, tempMaterial))
        {
            far = tempHitRecord.m_t;
            hitAnything = true;
            rec = tempHitRecord;
            outRef = tempObjectRef;
            outMaterial = tempMaterial;
        }
    }
    return hitAnything;
}

void Scene::CalculatePixel(const Camera& camera, int width, int height, uint32_t x, uint32_t y, uint32_t* buffer, const glm::mat4& invMat)
{
    auto pixel = buffer;
    pixel += y * width + x;

    float xPoint = (float(x + 0.5f) / width) * 2 - 1;
    float yPoint = (float(height - y - 0.5f) / height) * 2 - 1;
    glm::vec4 nearPoint = invMat * glm::vec4(xPoint, yPoint, -1, 1);
    glm::vec4 farPoint = invMat * glm::vec4(xPoint, yPoint, 1, 1);
    farPoint /= farPoint.w;
    nearPoint /= nearPoint.w;
    Ray ray = Ray(nearPoint, farPoint - nearPoint);
    *pixel = GetColor(ray, camera);
}

uint32_t Scene::GetColor(const Ray& ray, const Camera& camera)
{
    HitRecord resultHitRecord;
    ObjectRef resultObjectRef;
    std::shared_ptr<Material> resultMaterial;
    std::shared_ptr<LightMat> resultLightMat;


    if (CheckWorldHitObjects(ray, camera, camera.m_farPlaneDist, resultObjectRef, resultHitRecord, resultMaterial))
    {
        if (CheckWorldHitLights(ray, camera, resultHitRecord.m_t, resultObjectRef, resultHitRecord, resultLightMat))
        {
            return resultLightMat->m_color;
        }
        else
        {
            return GetLightedColor(resultHitRecord, resultMaterial, camera);
        } 
    }
    else if (CheckWorldHitLights(ray, camera, camera.m_farPlaneDist, resultObjectRef, resultHitRecord, resultLightMat))
    {
        return resultLightMat->m_color;
    }
    else
    {
        const glm::vec3 unit_direction = glm::normalize(ray.m_direction);
        const auto gradCoef = 0.5 * (unit_direction.y + 1.0);
        uint32_t gradientColor = 0xff << 24 |
            (uint32_t)(0x88) << 16 |
            (uint32_t)(0x77 * gradCoef) << 8 |
            (uint32_t)(0xcc * (1.0f - gradCoef));

        return gradientColor;
    }
}

uint32_t Scene::GetLightedColor(const HitRecord& rec, std::shared_ptr<Material>& material, const Camera& camera)
{
    HitRecord tempHitRecord;
    ObjectRef tempObjectRef;
    std::shared_ptr<Material> tempMaterial;

    uint32_t baseR = (material->m_baseColor >> 16) & 0xff;
    uint32_t baseG = (material->m_baseColor >> 8) & 0xff;
    uint32_t baseB = material->m_baseColor & 0xff;

    uint32_t specR = (material->m_specColor >> 16) & 0xff;
    uint32_t specG = (material->m_specColor >> 8) & 0xff;
    uint32_t specB = material->m_specColor & 0xff;

    uint32_t resultR = baseR * 0.1f;
    uint32_t resultG = baseG * 0.1f;
    uint32_t resultB = baseB * 0.1f;

    for(auto& light : m_pointLights)
    {
        auto dir = light.m_position - rec.m_point;
        float distance = glm::length(dir);
        Ray r = Ray(rec.m_point + rec.m_normal*0.0001f, dir);
        if (!CheckWorldHitObjects(r, camera, glm::length(dir), tempObjectRef, tempHitRecord, tempMaterial))
        {
            float attenuation = 1.0 / (1.0f + 0.009f * distance + 0.0032f * (distance * distance));

            float diffShadingFactor = std::max({ glm::dot(r.m_direction, rec.m_normal), 0.0001f });
            float specDot = glm::dot(glm::normalize((camera.m_transform.position - rec.m_point) + (light.m_position - rec.m_point)), rec.m_normal);
            //multiply by diffShadingFactor to avoid specular spots in dark areas far from the source of light and lower intensity on the light/shadow transition edge in case of sphere
            float specShadingFactor = pow(std::max({ specDot, 0.0001f }), 64.0f) * diffShadingFactor;

            uint32_t red = (light.m_light->m_color >> 16) & 0xff;
            uint32_t green = (light.m_light->m_color >> 8) & 0xff;
            uint32_t blue = light.m_light->m_color & 0xff;
            resultR += attenuation * red * (baseR * diffShadingFactor + specR * specShadingFactor) * light.m_light->m_strength / 0xff;
            resultG += attenuation * green * (baseG * diffShadingFactor + specG * specShadingFactor) * light.m_light->m_strength / 0xff;
            resultB += attenuation * blue * (baseB * diffShadingFactor + specB * specShadingFactor) * light.m_light->m_strength / 0xff;
        }
    }
    for (auto& light : m_spotLights)
    {
        auto dir = light.m_position - rec.m_point;
        float distance = glm::length(dir);
        Ray r = Ray(rec.m_point + rec.m_normal * 0.0001f, dir);
        if (!CheckWorldHitObjects(r, camera, distance, tempObjectRef, tempHitRecord, tempMaterial))
        {
            float cutOffOuter = glm::cos(light.m_angle);
            float cutOffInner = glm::cos(light.m_angle - glm::radians(5.0f));
            float theta = glm::dot(-glm::normalize(dir), glm::normalize(light.m_direction));
            float attenuation = 1.0 / (1.0f + 0.03f * distance + 0.01f * (distance * distance));
            if (theta <= cutOffOuter) 
            { 
                continue;
            }
            float epsilon = cutOffInner - cutOffOuter;
            float intensity = std::clamp((theta - cutOffOuter) / epsilon, 0.0f, 1.0f);

            float diffShadingFactor = std::max({ glm::dot(r.m_direction, rec.m_normal), 0.0001f });
            float specDot = glm::dot(glm::normalize((camera.m_transform.position - rec.m_point) + (light.m_position - rec.m_point)), rec.m_normal);
            float specShadingFactor = pow(std::max({ specDot, 0.0001f }), 64.0f) * diffShadingFactor;

            uint32_t red = (light.m_light->m_color >> 16) & 0xff;
            uint32_t green = (light.m_light->m_color >> 8) & 0xff;
            uint32_t blue = light.m_light->m_color & 0xff;
            resultR += intensity * attenuation * red * (baseR * diffShadingFactor + specR * specShadingFactor) * light.m_light->m_strength / 0xff;
            resultG += intensity * attenuation * green * (baseG * diffShadingFactor + specG * specShadingFactor) * light.m_light->m_strength / 0xff;
            resultB += intensity * attenuation * blue * (baseB * diffShadingFactor + specB * specShadingFactor) * light.m_light->m_strength / 0xff;
        }
    }
    for (auto& light : m_directLights)
    {
        auto dir = -light.m_direction;
        Ray r = Ray(rec.m_point + rec.m_normal * 0.0001f, dir);
        if (!CheckWorldHitObjects(r, camera, 1000.0f, tempObjectRef, tempHitRecord, tempMaterial))
        {
            float diffShadingFactor = std::max({ glm::dot(r.m_direction, rec.m_normal), 0.0001f });
            float specDot = glm::dot(glm::normalize(glm::normalize(camera.m_transform.position - rec.m_point) + glm::normalize(dir)), rec.m_normal);
            float specShadingFactor = pow(std::max({ specDot, 0.0001f }), 64.0f) * diffShadingFactor;

            uint32_t red = (light.m_light->m_color >> 16) & 0xff;
            uint32_t green = (light.m_light->m_color >> 8) & 0xff;
            uint32_t blue = light.m_light->m_color & 0xff;
            resultR += red * (baseR * diffShadingFactor + specR * specShadingFactor) * light.m_light->m_strength / 0xff;
            resultG += green * (baseG * diffShadingFactor + specG * specShadingFactor) * light.m_light->m_strength / 0xff;
            resultB += blue * (baseB * diffShadingFactor + specB * specShadingFactor) * light.m_light->m_strength / 0xff;
        }
    }

    resultR = resultR > 0xff ? 0xff : resultR;
    resultG = resultG > 0xff ? 0xff : resultG;
    resultB = resultB > 0xff ? 0xff : resultB;
    
    return resultR << 16 | resultG << 8 | resultB;
}

IObjectMover* Scene::CastRayToMove(const Camera& camera, HitRecord& rec, int width, int height, uint32_t x, uint32_t y)
{
    HitRecord resultHitRecord;
    ObjectRef resultObjectRef;
    std::shared_ptr<Material> resultMaterial;

    auto view = camera.generateViewMat();
    auto proj = camera.generateProjMat(float(width) / height);
    auto invMat = glm::inverse(proj * view);

    float xPoint = (float(x + 0.5f) / width) * 2 - 1;
    float yPoint = (float(height - y - 0.5f) / height) * 2 - 1;
    glm::vec4 nearPoint = invMat * glm::vec4(xPoint, yPoint, -1, 1);
    glm::vec4 farPoint = invMat * glm::vec4(xPoint, yPoint, 1, 1);
    farPoint /= farPoint.w;
    nearPoint /= nearPoint.w;
    Ray ray = Ray(nearPoint, farPoint - nearPoint);

    if (CheckWorldHitObjects(ray, camera, camera.m_farPlaneDist, resultObjectRef, resultHitRecord, resultMaterial))
    {
        rec = resultHitRecord;
    }
    IObjectMover* result = nullptr;
    switch (resultObjectRef.m_type)
    {
    case IntersectedType::Mesh:
    {
        MeshMover* mover = new MeshMover;
        mover->mesh = static_cast<MeshEntity*>(resultObjectRef.m_object)->m_mesh.get();
        result = mover;
        break;
    }
    case IntersectedType::Sphere:
    {
        SphereMover* mover = new SphereMover;
        mover->sphere = static_cast<Sphere*>(resultObjectRef.m_object);
        result = mover;
        break;
    }
    case IntersectedType::Triangle:
    {
        TriangleMover* mover = new TriangleMover;
        mover->triangle = static_cast<Triangle*>(resultObjectRef.m_object);
        result = mover;
        break;
    }
    case IntersectedType::Plane:
    {
        PlaneMover* mover = new PlaneMover;
        mover->plane = static_cast<Plane*>(resultObjectRef.m_object);
        result = mover;
        break;
    }
    }
    return result;
}
