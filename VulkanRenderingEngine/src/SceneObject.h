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
	SceneObject(uint64_t ID);
	~SceneObject();

	void SetParent(SceneObject* newParent);
	void RemoveChild(SceneObject* newParent);
	void AddChilds(SceneObject* newChild);

	uint64_t GetID() const;

	virtual nlohmann::json Save();
	virtual void Load(nlohmann::json scene);

private:

};