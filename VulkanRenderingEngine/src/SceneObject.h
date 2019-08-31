#pragma once
#include "Transform.h"
#include "SceneObjectReference.h"

#include <unordered_map>
#include <optional>
#include <json.hpp>

class SceneObject
{
private:
	uint64_t ID;

public:
	std::string name = "Unamed";
	Transform transform;
	std::optional<SceneObjectReference> parent;
	std::unordered_map<uint64_t, SceneObjectReference> childs;

	SceneObject();
	SceneObject(nlohmann::json sceneObject);
	~SceneObject();

	void SetParent(SceneObject* newParent);
	void RemoveChild(SceneObject* newParent);
	void AddChilds(SceneObject* newChild);

	uint64_t GetID() const;

	virtual nlohmann::json Save();

private:
	virtual void Load(nlohmann::json sceneObject);

private:

};