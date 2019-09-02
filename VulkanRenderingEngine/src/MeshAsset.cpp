#include "MeshAsset.h"

std::string MeshAsset::GetType()
{
	return "MeshAsset";
}

void MeshAsset::GUI()
{
	Asset::GUI();
	ImGui::Text("Ima mesh asset :)");
}

nlohmann::json MeshAsset::Save()
{
	using json = nlohmann::json;

	json meshAsset = Asset::Save();

	return meshAsset;
}

void MeshAsset::Load(nlohmann::json data)
{
	using json = nlohmann::json;

	Asset::Load(data);
}
