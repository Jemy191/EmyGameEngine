#pragma once
#include "SceneObject.h"
#include "Model.h"
#include <memory>

class SceneModel : public SceneObject
{
private:
	Model* model = nullptr;

	std::string meshToLoadInput;
	std::string textureToLoadInput;

public:
	SceneModel();
	virtual ~SceneModel() override;

	virtual nlohmann::json Save() override;
	virtual void Load(nlohmann::json sceneModel) override;

	virtual std::string GetType() override;

	virtual void Update() override;
	virtual void GUI() override;

private:
};