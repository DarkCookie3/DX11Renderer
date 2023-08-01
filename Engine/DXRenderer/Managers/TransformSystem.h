#pragma once
#include "../Utils/SolidVector/SolidVector.h"
#include "../CustomTypes/TransformDX.h"

class TransformDX_ID;

class TransformSystem
{
public:

	static TransformSystem& GetInstance()
	{
		static TransformSystem instance;
		return instance;
	}
	TransformSystem(const TransformSystem&) = delete;
	TransformSystem& operator=(const TransformSystem&) = delete;

	TransformDX& GetTransform(TransformDX_ID& ID);
	TransformDX_ID CreateTransform(const TransformDX& transform);
	void RemoveTransform(TransformDX_ID& ID);
private:
	SolidVector<TransformDX> m_transformVector;
	TransformSystem() {}
};

class TransformDX_ID
{
public:
	TransformDX_ID(const TransformDX& arg) : counter(new uint32_t(0))
	{
		auto transf = TransformSystem::GetInstance().CreateTransform(arg);
		m_id = transf.m_id;
		counter = transf.counter;
	}
	TransformDX_ID& operator=(const TransformDX_ID& arg)
	{
		counter = arg.counter;
		m_id = arg.m_id;
		return *this;
	}
	TransformDX_ID(const TransformDX_ID& arg) : m_id(arg.m_id), counter(arg.counter)
	{
	}

	~TransformDX_ID()
	{
		if (counter.use_count() <= 1)
		{
			TransformSystem::GetInstance().RemoveTransform(*this);
		}
	}

	TransformDX& GetTransform()
	{
		return TransformSystem::GetInstance().GetTransform(*this);
	}

	bool operator ==(const TransformDX_ID& transformID)
	{
		return this->m_id == transformID.m_id;
	}

	friend class TransformSystem;
private:
	TransformDX_ID(uint32_t id) : m_id(id), counter(new uint32_t(0)) {}
	operator uint32_t() const { return m_id; }
	uint32_t m_id;
	std::shared_ptr<uint32_t> counter;
};