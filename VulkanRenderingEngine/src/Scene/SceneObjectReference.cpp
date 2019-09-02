#include "SceneObject.h"
#include "SceneObjectReference.h"
#include "Scene.h"

SceneObjectReference::SceneObjectReference(uint64_t sceneObjectID)
{
	this->sceneObjectID = sceneObjectID;
}

SceneObjectReference::SceneObjectReference(SceneObject* referencedSceneObject)
{
	this->sceneObjectID = referencedSceneObject->GetID();
	this->referencedSceneObject = referencedSceneObject;
}

SceneObjectReference::SceneObjectReference()
{
}

uint64_t SceneObjectReference::GetSceneObjectID() const
{
	return sceneObjectID;
}

SceneObject* SceneObjectReference::GetReferencedSceneObject()
{
	if (referencedSceneObject == nullptr)
	{
		referencedSceneObject = Scene::GetCurrentScene()->GetSceneObjectByID(sceneObjectID);
	}
	return referencedSceneObject;
}
