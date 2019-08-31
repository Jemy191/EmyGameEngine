#include "SceneObject.h"
#include "Log.h"
#include "Scene.h"

SceneObject::SceneObject()
{
	ID = Scene::GetIDCounter();
	Logger::Log("SceneObject created with id: " + std::to_string(ID));
	Scene::IncrementIDCounter();
}

SceneObject::SceneObject(uint64_t ID)
{
	this->ID = ID;
	Logger::Log("SceneObject loaded with id: " + std::to_string(ID));
}

SceneObject::~SceneObject()
{
}

void SceneObject::SetParent(SceneObject* newParent)
{
	if (parent.has_value())
	{
		parent.value().GetReferencedSceneObject()->RemoveChild(this);
	}

	if (newParent == nullptr)
	{
		parent.reset();
		Scene::GetCurrentScene()->AddToRootObject(this);
	}
	else
		parent.emplace(SceneObjectReference(newParent));
}

void SceneObject::RemoveChild(SceneObject* child)
{
	childs.erase(child->GetID());
	child->SetParent(nullptr);
}

void SceneObject::AddChilds(SceneObject* newChild)
{
	childs.emplace(newChild->GetID(), SceneObjectReference(newChild));
}

uint64_t SceneObject::GetID() const
{
	return ID;
}

nlohmann::json SceneObject::Save()
{
	using json = nlohmann::json;

	json sceneObject;

	sceneObject["ID"] = ID;
	sceneObject["name"] = name;

	return sceneObject;
}

void SceneObject::Load(nlohmann::json scene)
{
}
