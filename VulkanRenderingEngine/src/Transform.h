#pragma once
#include <glm/glm.hpp>

class Transform
{
public:
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);

	Transform();
	~Transform();

private:

};