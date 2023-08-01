#pragma once
#include <cstdint>
#include "../CustomTypes/InstancedMesh.h"

InstancedMesh<Vertex> GetUnitSphere()
{
	const uint32_t SIDES = 6;
	const uint32_t GRID_SIZE = 12;
	const uint32_t TRIS_PER_SIDE = GRID_SIZE * GRID_SIZE * 2;
	const uint32_t VERT_PER_SIZE = (GRID_SIZE + 1) * (GRID_SIZE + 1);

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	vertices.resize(VERT_PER_SIZE * SIDES);

	int sideMasks[6][3] =
	{
		{ 2, 1, 0 },
		{ 0, 1, 2 },
		{ 2, 1, 0 },
		{ 0, 1, 2 },
		{ 0, 2, 1 },
		{ 0, 2, 1 }
	};

	float sideSigns[6][3] =
	{
		{ +1, +1, +1 },
		{ -1, +1, +1 },
		{ -1, +1, -1 },
		{ +1, +1, -1 },
		{ +1, -1, -1 },
		{ +1, +1, +1 }
	};

	int vertexIndex = 0;
	for (int side = 0; side < SIDES; ++side)
	{
		for (int row = 0; row < GRID_SIZE + 1; ++row)
		{
			for (int col = 0; col < GRID_SIZE + 1; ++col)
			{
				XMVECTOR v;
				v.m128_f32[0] = col / float(GRID_SIZE) * 2.f - 1.f;
				v.m128_f32[1] = row / float(GRID_SIZE) * 2.f - 1.f;
				v.m128_f32[2] = 1.f;

				vertices[vertexIndex].Position.m128_f32[sideMasks[side][0]] = v.m128_f32[0] * sideSigns[side][0];
				vertices[vertexIndex].Position.m128_f32[sideMasks[side][1]] = v.m128_f32[1] * sideSigns[side][1];
				vertices[vertexIndex].Position.m128_f32[sideMasks[side][2]] = v.m128_f32[2] * sideSigns[side][2];
				vertices[vertexIndex].Normal = vertices[vertexIndex].Position;
				vertices[vertexIndex].Position = XMVector3Normalize(vertices[vertexIndex].Position);
				vertices[vertexIndex].Position.m128_f32[3] = 1;
				vertices[vertexIndex].Normal.m128_f32[3] = 0;

				vertexIndex += 1;
			}
		}
	}

	indices.resize(TRIS_PER_SIDE * SIDES * 3);

	int indexIndex = 0;
	for (int side = 0; side < SIDES; ++side)
	{
		uint32_t sideOffset = VERT_PER_SIZE * side;

		for (int row = 0; row < GRID_SIZE; ++row)
		{
			for (int col = 0; col < GRID_SIZE; ++col)
			{
				indices[indexIndex] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 0;
				indices[indexIndex + 1] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 0;
				indices[indexIndex + 2] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 1;

				indices[3 + indexIndex] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 0;
				indices[3 + indexIndex + 1] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 1;
				indices[3 + indexIndex + 2] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 1;

				indexIndex += 2 * 3;
			}
		}
	}

	return InstancedMesh("unitSphereMesh", vertices, indices);
}