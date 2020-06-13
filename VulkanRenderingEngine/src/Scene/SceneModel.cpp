#include "Scene/SceneModel.h"
#include "Rendering/Vulkan/VulkanRenderer.h"

SceneModel::~SceneModel()
{
	VulkanRenderer::GetInstance()->MarkModelToBeRemove(model.get());
}

nlohmann::json SceneModel::Save()
{
	nlohmann::json sceneModel = SceneObject::Save();

	sceneModel["Model"]["ModelSaved"] = model.get() != nullptr;
	if (model)
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
	SceneObject::Update();
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
			VulkanRenderer::GetInstance()->MarkModelToBeRemove(model.get());

		model.reset(VulkanRenderer::GetInstance()->BasicLoadModel(std::string(meshToLoadInput), std::string(textureToLoadInput), glm::vec3(0), glm::vec3(0), glm::vec3(1)));
	}
}

void SceneModel::Load(nlohmann::json sceneModel)
{
	SceneObject::Load(sceneModel);

	if (sceneModel["Model"]["ModelSaved"])
		model.reset(VulkanRenderer::GetInstance()->BasicLoadModel(sceneModel["Model"]["Mesh"], sceneModel["Model"]["Texture"], transform.position, transform.rotation, transform.scale));
}
