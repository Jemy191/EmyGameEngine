#pragma once
#include "SceneObject.h"
#include <unordered_map>
#include <vector>

class SceneObject;

class Scene
{
private:
	static Scene* currentScene;
	static uint64_t IDCounter;
	static const std::string PATH;

	std::unordered_map<uint64_t, SceneObject*> sceneObjects;
	std::vector<SceneObject*> sceneObjectAtRoot;

	std::string name;

public:
	Scene(std::string name);
	~Scene();

	void Add(SceneObject* sceneObject);
	void AddToRootObject(SceneObject* sceneObject);
	void Remove(SceneObject* sceneObject);

	static Scene* GetCurrentScene();
	SceneObject* GetSceneObjectByID(uint64_t ID);
	SceneObject* GetRootSceneObject(int index);
	int GetRootSceneObjectSize();

	static uint64_t GetIDCounter();
	static void IncrementIDCounter();

	void Save();
	void Load();
};