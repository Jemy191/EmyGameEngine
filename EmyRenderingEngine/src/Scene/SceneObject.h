#pragma once
#include "Game/Transform.h"
#include "SceneObjectReference.h"

#include <unordered_map>
#include <optional>
#include <json.hpp>
#include "Helper/Log.h"
#include "Rendering/UI/ImguiBase.h"

class SceneObject
{
private:
	static bool isBeingCreated;
	bool hasBeenInit = false;

	uint64_t ID = 0;

public:
	std::string name = "Unamed";
	Transform transform;
	std::optional<SceneObjectReference> parent;
	std::unordered_map<uint64_t, SceneObjectReference> childs;

	virtual void Init();
	virtual void Init(nlohmann::json data);
	SceneObject();
	virtual ~SceneObject();

	void SetParent(SceneObject* newParent);
	void RemoveChild(SceneObject* newParent);
	void AddChilds(SceneObject* newChild);

	uint64_t GetID() const;
	virtual std::string GetType();

	virtual nlohmann::json Save();

	virtual void GUI();
	virtual void Update();

	static SceneObject* LoadByType(nlohmann::json data);
	
	template<class T>
	static SceneObject* Create();

protected:
	virtual void Load(nlohmann::json sceneObject);

private:
	void MarkToRemove(SceneObject* sceneObject);

};

template<class T>
SceneObject* SceneObject::Create()
{
	isBeingCreated = true;
	SceneObject* object = new T();
	isBeingCreated = false;

	object->Init();
	return object;
}
