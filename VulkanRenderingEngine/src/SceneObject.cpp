#include "SceneObject.h"
#include "Log.h"
#include "Scene.h"

SceneObject::SceneObject()
{
	ID = Scene::GetIDCounter();
	Scene::IncrementIDCounter();
}

SceneObject::SceneObject(nlohmann::json sceneObject)
{
	Load(sceneObject);
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
	sceneObject["Name"] = name;
	sceneObject["Transform"] = transform.Save();

	return sceneObject;
}

void SceneObject::Load(nlohmann::json sceneObject)
{
	ID = sceneObject["ID"];
	name = sceneObject["Name"];

	nlohmann::json transformData = sceneObject["Transform"];
	transform = Transform(transformData);
}
