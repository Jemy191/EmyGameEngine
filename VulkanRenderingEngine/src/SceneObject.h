#pragma once
#include "Transform.h"
#include "SceneObjectReference.h"

#include <unordered_map>
#include <optional>
#include <json.hpp>
#include "Log.h"

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
	virtual ~SceneObject();

	void SetParent(SceneObject* newParent);
	void RemoveChild(SceneObject* newParent);
	void AddChilds(SceneObject* newChild);

	uint64_t GetID() const;
	virtual std::string GetType();

	virtual nlohmann::json Save();
	virtual void Load(nlohmann::json sceneObject);

	virtual void GUI();
	virtual void Update();

	static SceneObject* LoadByType(nlohmann::json data);

private:

	void MarkToRemove(SceneObject* sceneObject);

private:

};