#pragma once

enum class IntersectedType { None = 0, Sphere, Triangle, Plane, Mesh, PointLight, SpotLight};

struct ObjectRef
{
	void* m_object = nullptr;
	IntersectedType m_type = IntersectedType::None;
};
