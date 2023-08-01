#include "TransformSystem.h"

TransformDX& TransformSystem::GetTransform(TransformDX_ID& ID)
{
	return m_transformVector[ID];
}

TransformDX_ID TransformSystem::CreateTransform(const TransformDX& transform)
{
	return m_transformVector.insert(transform);
}

void TransformSystem::RemoveTransform(TransformDX_ID& ID)
{
	m_transformVector.erase(ID);
}
