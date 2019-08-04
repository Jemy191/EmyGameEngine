#pragma once
#include "Model.h"

class Scene
{
public:
	Scene(std::string fileName);
	~Scene();

private:
	std::vector<Model*> models = std::vector<Model*>();
};