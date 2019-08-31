#include "Scene.h"
#include <json.hpp>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "Log.h"

Scene* Scene::currentScene = nullptr;
uint64_t Scene::IDCounter = 0;

const std::string Scene::PATH = "Assets/Scenes/";

Scene::Scene(std::string name)
{
	this->name = name;
	currentScene = this;
	Load();
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
	for (auto it = sceneObjectAtRoot.begin(); it != sceneObjectAtRoot.end(); it++)
	{
		if ((*it) == sceneObject)
		{
			return;
		}
	}
	sceneObjectAtRoot.push_back(sceneObject);
}

void Scene::Remove(SceneObject* sceneObject)
{
	if (sceneObject == nullptr)
		return;

	sceneObjects.erase(sceneObject->GetID());
	sceneObject->SetParent(nullptr);
	for (auto it = sceneObjectAtRoot.begin(); it != sceneObjectAtRoot.end(); it++)
	{
		if ((*it) == sceneObject)
		{
			sceneObjectAtRoot.erase(it);
			Logger::Log("Scene object remove from scene.");
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

	json outScene;

	outScene["Scene"]["IDCounter"] = IDCounter;

	for (auto it = sceneObjects.begin(); it != sceneObjects.end(); it++)
	{
		outScene["Scene"]["SceneObject"].push_back((*it).second->Save());
	}

	// save scene
	std::ofstream output(PATH + name + ".json");
	output << std::setw(4) << outScene;
}

void Scene::Load()
{
	using json = nlohmann::json;

	std::ifstream input("Assets/Scenes/" + name + ".json");
	
	if (input.fail())
	{
		Logger::Log("Unable to open " + name + ".json scene file");
		return;
	}

	json scene;
	input >> scene;

	IDCounter = scene["Scene"]["IDCounter"];

	for (auto sceneObjectData : scene["Scene"]["SceneObject"])
	{
		Add(new SceneObject(sceneObjectData));
	}
}
