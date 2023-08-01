#pragma once
#include <vector>
#include "../Utils/d3d.h"


class IndexBuffer
{
public:
	IndexBuffer() = default;
	IndexBuffer(const std::vector<uint32_t>& indices);
	~IndexBuffer();

	void Bind();

private:
	DxResPtr<ID3D11Buffer> m_buffer;
};