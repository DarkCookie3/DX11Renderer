#pragma once
#include <vector>
#include "../CustomTypes/Vertex.h"

struct Cube
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

inline Cube GetUnitCubeIndexed()
{
	Cube result;

	//front
	result.vertices.push_back({ { -1, -1, -1, 1 },  { 0, 0, -1, 0 }, { 0, -1, 0, 0 }, { -1, 0, 0, 0 }, {0, 0}});
	result.vertices.push_back({ { -1, 1, -1, 1 },  { 0, 0, -1, 0 }, { 0, -1, 0, 0 }, { -1, 0, 0, 0 }, {0, 1}});
	result.vertices.push_back({ { 1, 1, -1, 1 },  { 0, 0, -1, 0 }, { 0, -1, 0, 0 }, { -1, 0, 0, 0 }, {1, 1}});
	result.vertices.push_back({ { 1, -1, -1, 1 },  { 0, 0, -1, 0 }, { 0, -1, 0, 0 }, { -1, 0, 0, 0 }, {1, 0}});

	//back
	result.vertices.push_back({ { -1, -1, 1, 1 },  { 0, 0, 1, 0 }, { 0, -1, 0, 0 }, { 1, 0, 0, 0 }, {1, 0}});
	result.vertices.push_back({ { 1, -1, 1, 1 },  { 0, 0, 1, 0 }, { 0, -1, 0, 0 }, { 1, 0, 0, 0 }, {0, 0}});
	result.vertices.push_back({ { 1, 1, 1, 1 },  { 0, 0, 1, 0 }, { 0, -1, 0, 0 }, { 1, 0, 0, 0 }, {0, 1}});
	result.vertices.push_back({ { -1, 1, 1, 1 },  { 0, 0, 1, 0 }, { 0, -1, 0, 0 }, { 1, 0, 0, 0 }, {1, 1}});

	//right
	result.vertices.push_back({ { 1, -1, -1, 1 },  { 1, 0, 0, 0 }, { 0, -1, 0, 0 }, { 0, 0, -1, 0 }, {1, 0}});
	result.vertices.push_back({ { 1, 1, -1, 1 },  { 1, 0, 0, 0 }, { 0, -1, 0, 0 }, { 0, 0, -1, 0 }, {1, 1}});
	result.vertices.push_back({ { 1, 1, 1, 1 },  { 1, 0, 0, 0 }, { 0, -1, 0, 0 }, { 0, 0, -1, 0 }, {0, 1}});
	result.vertices.push_back({ { 1, -1, 1, 1 },  { 1, 0, 0, 0 }, { 0, -1, 0, 0 }, { 0, 0, -1, 0 }, {0, 0}});

	//left
	result.vertices.push_back({ { -1, -1, -1, 1 },  { -1, 0, 0, 0 }, { 0, -1, 0, 0 }, { 0, 0, 1, 0 }, {0, 0}});
	result.vertices.push_back({ { -1, -1, 1, 1 },  { -1, 0, 0, 0 }, { 0, -1, 0, 0 }, { 0, 0, 1, 0 }, {1, 0}});
	result.vertices.push_back({ { -1, 1, 1, 1 },  { -1, 0, 0, 0 }, { 0, -1, 0, 0 }, { 0, 0, 1, 0 }, {1, 1}});
	result.vertices.push_back({ { -1, 1, -1, 1 },  { -1, 0, 0, 0 }, { 0, -1, 0, 0 }, { 0, 0, 1, 0 }, {0, 1}});

	//top
	result.vertices.push_back({ { -1, 1, -1, 1 },  { 0, 1, 0, 0 }, { 0, 0, -1, 0 }, { 1, 0, 0, 0 }, {0, 1}});
	result.vertices.push_back({ { -1, 1, 1, 1 },  { 0, 1, 0, 0 }, { 0, 0, -1, 0 }, { 1, 0, 0, 0 }, {0, 0}});
	result.vertices.push_back({ { 1, 1, 1, 1 },  { 0, 1, 0, 0 }, { 0, 0, -1, 0 }, { 1, 0, 0, 0 }, {1, 0}});
	result.vertices.push_back({ { 1, 1, -1, 1 },  { 0, 1, 0, 0 }, { 0, 0, -1, 0 }, { 1, 0, 0, 0 }, {1, 1}});

	//bottom
	result.vertices.push_back({ { -1, -1, -1, 1 },  { 0, -1, 0, 0 }, { 0, 0, -1, 0 }, { -1, 0, 0, 0 }, {0, 0}});
	result.vertices.push_back({ { 1, -1, -1, 1 },  { 0, -1, 0, 0 }, { 0, 0, -1, 0 }, { -1, 0, 0, 0 }, {1, 0}});
	result.vertices.push_back({ { 1, -1, 1, 1 },  { 0, -1, 0, 0 }, { 0, 0, -1, 0 }, { -1, 0, 0, 0 }, {1, 1}});
	result.vertices.push_back({ { -1, -1, 1, 1 },  { 0, -1, 0, 0 }, { 0, 0, -1, 0 }, { -1, 0, 0, 0 }, {0, 1}});

	result.indices.insert(result.indices.end(), { 0, 1, 2, 0, 2, 3 });
	result.indices.insert(result.indices.end(), { 4, 5, 6, 4, 6, 7 });

	result.indices.insert(result.indices.end(), { 8, 9, 10, 8, 10, 11 });
	result.indices.insert(result.indices.end(), { 12, 13, 14, 12, 14, 15 });

	result.indices.insert(result.indices.end(), { 16, 17, 18, 16, 18, 19 });
	result.indices.insert(result.indices.end(), { 20, 21, 22, 20, 22, 23 });

	return result;
}