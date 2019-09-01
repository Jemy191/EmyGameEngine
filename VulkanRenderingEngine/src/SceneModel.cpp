#include "SceneModel.h"
#include "ImguiStuff.h"
#include "VulkanManager.h"

SceneModel::SceneModel() : SceneObject()
{
}

SceneModel::~SceneModel()
{
	VulkanManager::GetInstance()->MarkModelToBeRemove(model);
}

nlohmann::json SceneModel::Save()
{
	nlohmann::json sceneModel = SceneObject::Save();

	sceneModel["Model"]["ModelSaved"] = model != nullptr;
	if (model != nullptr)
	{
		sceneModel["Model"]["Mesh"] = model->meshName;
		sceneModel["Model"]["Texture"] = model->textureName;
	}

	return sceneModel;
}

std::string SceneModel::GetType()
{
	return "SceneModel";
}

void SceneModel::Update()
{
	if (model != nullptr)
	{
		model->position = transform.position;
		model->rotation = transform.rotation;
		model->scale = transform.scale;
	}
}

void SceneModel::GUI()
{
	SceneObject::GUI();

	ImGui::InputText("MeshToLoad", &meshToLoadInput);
	ImGui::InputText("TextureToLoad", &textureToLoadInput);

	std::string buttonText = model == nullptr ? "Load model" : "Reload model";

	if (ImGui::Button(buttonText.c_str()))
	{
		if (model != nullptr)
			VulkanManager::GetInstance()->MarkModelToBeRemove(model);

		model = VulkanManager::GetInstance()->BasicLoadModel(std::string(meshToLoadInput), std::string(textureToLoadInput), glm::vec3(0), glm::vec3(0), glm::vec3(1));
	}
}

void SceneModel::Load(nlohmann::json sceneModel)
{
	SceneObject::Load(sceneModel);

	if (sceneModel["Model"]["ModelSaved"])
		model = VulkanManager::GetInstance()->BasicLoadModel(sceneModel["Model"]["Mesh"], sceneModel["Model"]["Texture"], transform.position, transform.rotation, transform.scale);
}
