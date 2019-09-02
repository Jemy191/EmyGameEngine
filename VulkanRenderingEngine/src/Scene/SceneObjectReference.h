#pragma once

class SceneObject;

class SceneObjectReference
{
	uint64_t sceneObjectID = 0;
	SceneObject* referencedSceneObject = nullptr;

public:
	SceneObjectReference(uint64_t sceneObjectID);
	SceneObjectReference(SceneObject* referencedSceneObject);

	SceneObjectReference();

	uint64_t GetSceneObjectID() const;
	SceneObject* GetReferencedSceneObject();
};