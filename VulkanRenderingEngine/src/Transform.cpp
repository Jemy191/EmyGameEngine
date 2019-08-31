#include "Transform.h"
#include "ImguiStuff.h"

Transform::Transform()
{
}

Transform::Transform(nlohmann::json transform)
{
	glm::vec3 position(transform["position"]["x"], transform["position"]["y"], transform["position"]["z"]);
	glm::vec3 rotation(transform["rotation"]["x"], transform["rotation"]["y"], transform["rotation"]["z"]);
	glm::vec3 scale(transform["scale"]["x"], transform["scale"]["y"], transform["scale"]["z"]);
}

Transform::~Transform()
{
}

nlohmann::json Transform::Save()
{
	nlohmann::json transform;
	transform["position"]["x"] = position.x;
	transform["position"]["y"] = position.y;
	transform["position"]["z"] = position.z;

	transform["rotation"]["x"] = rotation.x;
	transform["rotation"]["y"] = rotation.y;
	transform["rotation"]["z"] = rotation.z;

	transform["scale"]["x"] = scale.x;
	transform["scale"]["y"] = scale.y;
	transform["scale"]["z"] = scale.z;

	return transform;
}

void Transform::GUI()
{
	ImGui::PushID(this);// push pointer as id
	ImGui::DragFloat3("Position", &position.x, 0.1f, -20, 20);
	ImGui::DragFloat3("rotation", &rotation.x, 0.1f, 0, 360);
	ImGui::DragFloat3("scale", &scale.x, 0.1f, -5, 5);
	ImGui::PopID();
}
