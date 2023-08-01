#pragma once
#include "../../Submodules/glm/glm/gtx/quaternion.hpp"
#include "../../Submodules/glm/glm/gtc/quaternion.hpp"
#include "../../Submodules/glm/glm/glm.hpp"
#include <iostream>

struct Transform
{
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	glm::mat4 ToMat4() const
	{
		auto transform = glm::mat4(1.0f);
		transform = glm::translate(transform, position);
		transform = glm::scale(transform, scale);
		auto rotat = glm::toMat4(rotation);
		return transform * rotat;
	}

	glm::vec3 Right()
	{
		auto trans = glm::toMat3(rotation);
		return glm::vec3(trans[0][0], trans[0][1], trans[0][2]);
	}
	glm::vec3 Up()
	{
		auto trans = glm::toMat3(rotation);
		return glm::vec3(trans[1][0], trans[1][1], trans[1][2]);
	}
	glm::vec3 Forward()
	{
		auto trans = glm::toMat3(rotation);
		return glm::vec3(trans[2][0], trans[2][1], trans[2][2]);
	}
};
