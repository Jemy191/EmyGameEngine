#include "SceneObject.h"
#include "SceneModel.h"
#include "Scene.h"

bool SceneObject::isBeingCreated = false;
bool SceneObject::hasBeenInit = false;

void SceneObject::Init()
{
	if (hasBeenInit)
		Logger::Log(LogSeverity::ERROR, "SceneObject is already initialized.");

	hasBeenInit = true;

	ID = Scene::GetIDCounter();
	Scene::IncrementIDCounter();
}

void SceneObject::Init(nlohmann::json data)
{
	if (hasBeenInit)
		Logger::Log(LogSeverity::ERROR, "SceneObject is already initialized.");

	hasBeenInit = true;

	this->Load(data);
}

SceneObject::SceneObject()
{
	if (isBeingCreated == false)
		Logger::Log(LogSeverity::FATAL_ERROR, "You need to use SceneObject<T>::Create() or SceneObjectLoadByType() instead of the constructor.");
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

std::string SceneObject::GetType()
{
	return "SceneObject";
}

nlohmann::json SceneObject::Save()
{
	using json = nlohmann::json;

	json sceneObject;

	sceneObject["ClassType"] = GetType();
	sceneObject["ID"] = ID;
	sceneObject["Name"] = name;
	sceneObject["Transform"] = transform.Save();

	return sceneObject;
}

void SceneObject::GUI()
{
	if (ImGui::Button("Delete"))
		MarkToRemove(this);

	ImGui::Text(std::to_string(ID).c_str());
	ImGui::InputText("Name", &name);
	transform.GUI();
}

void SceneObject::Update()
{
}

SceneObject* SceneObject::LoadByType(nlohmann::json data)
{
	std::string type = data["ClassType"];

	SceneObject* returnObject = nullptr;

	isBeingCreated = true;

	if (type == "SceneObject")
		returnObject = new SceneObject();
	else if (type == "SceneModel")
		returnObject = new SceneModel();

	isBeingCreated = false;

	if (returnObject == nullptr)
		Logger::Log(LogSeverity::ERROR, "Scene Object of type: " + type + " not found");
	else
		returnObject->Init(data);

	return returnObject;
}

void SceneObject::Load(nlohmann::json sceneObject)
{
	ID = sceneObject["ID"];
	name = sceneObject["Name"];

	nlohmann::json transformData = sceneObject["Transform"];
	transform = Transform(transformData);
}

void SceneObject::MarkToRemove(SceneObject* sceneObject)
{
	Scene::GetCurrentScene()->MarkToRemove(sceneObject);
}
