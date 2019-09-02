#include "MeshAsset.h"

std::string ModelAsset::GetType()
{
	return "Mesh";
}

void ModelAsset::GUI()
{
	Asset::GUI();
	ImGui::Text("Ima mesh asset :)");
}

nlohmann::json ModelAsset::Save()
{
	using json = nlohmann::json;

	json meshAsset = Asset::Save();

	return meshAsset;
}

void ModelAsset::Load(nlohmann::json sceneObject)
{
	using json = nlohmann::json;
}
