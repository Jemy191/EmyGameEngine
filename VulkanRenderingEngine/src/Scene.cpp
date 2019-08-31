#include "Scene.h"
#include <json.hpp>
#include <fstream>
#include <iomanip>
#include <algorithm>

Scene* Scene::currentScene = nullptr;
uint64_t Scene::IDCounter = 0;

const std::string Scene::PATH = "Assets/Scenes/";

Scene::Scene(std::string name)
{
	this->name = name;
	currentScene = this;
}

Scene::~Scene()
{
}

void Scene::Add(SceneObject* sceneObject)
{
	sceneObjects.emplace(sceneObject->GetID(), sceneObject);
	if (!sceneObject->parent.has_value())
		AddToRootObject(sceneObject);
}

void Scene::AddToRootObject(SceneObject* sceneObject)
{
	sceneObjectAtRoot.push_back(sceneObject);
}

void Scene::Remove(SceneObject* sceneObject)
{
	sceneObjects.erase(sceneObject->GetID());
	sceneObject->SetParent(nullptr);
	for (auto it = sceneObjectAtRoot.begin(); it != sceneObjectAtRoot.end(); it++)
	{
		if ((*it) == sceneObject)
		{
			sceneObjectAtRoot.erase(it);
			break;
		}
	}
	delete sceneObject;
	sceneObject = nullptr;
}

Scene* Scene::GetCurrentScene()
{
	return currentScene;
}

SceneObject* Scene::GetSceneObjectByID(uint64_t ID)
{
	return sceneObjects[ID];
}

SceneObject* Scene::GetRootSceneObject(int index)
{
	return sceneObjectAtRoot[index];
}

int Scene::GetRootSceneObjectSize()
{
	return sceneObjectAtRoot.size();
}

uint64_t Scene::GetIDCounter()
{
	return IDCounter;
}

void Scene::IncrementIDCounter()
{
	IDCounter++;
}

void Scene::Save()
{
	using json = nlohmann::json;

	// Save scene
	json outScene;

	outScene["Scene"]["IDCounter"] = IDCounter;

	for (auto it = sceneObjects.begin(); it != sceneObjects.end(); it++)
	{
		outScene["Scene"]["Model"].push_back((*it).second->Save());
	}
	/*for (size_t i = 0; i < sceneObjects.size(); i++)
	{
		json model;
		model["position"]["x"] = vulkanManager.models[i]->position.x;
		model["position"]["y"] = vulkanManager.models[i]->position.y;
		model["position"]["z"] = vulkanManager.models[i]->position.z;

		model["rotation"]["x"] = vulkanManager.models[i]->rotation.x;
		model["rotation"]["y"] = vulkanManager.models[i]->rotation.y;
		model["rotation"]["z"] = vulkanManager.models[i]->rotation.z;

		model["scale"]["x"] = vulkanManager.models[i]->scale.x;
		model["scale"]["y"] = vulkanManager.models[i]->scale.y;
		model["scale"]["z"] = vulkanManager.models[i]->scale.z;

		model["mesh"] = vulkanManager.models[i]->meshName;
		model["texture"] = vulkanManager.models[i]->textureName;

		outScene["scene"]["models"].push_back(model);
	}*/

	// save scene
	std::ofstream output(PATH + name + ".json");
	output << std::setw(4) << outScene;
}

void Scene::Load()
{
}
