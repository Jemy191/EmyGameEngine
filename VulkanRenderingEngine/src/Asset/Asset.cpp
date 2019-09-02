#include "Asset.h"
#include "AssetManager.h"
#include <fstream>
#include <iomanip>
#include "MeshAsset.h"

bool Asset::isBeingCreated = false;
bool Asset::hasBeenInit = false;
const std::string Asset::PATH = "Assets/TempAssetFolder/";

Asset::Asset()
{
	if (isBeingCreated == false)
		Logger::Log(LogSeverity::FATAL_ERROR, "You need to use SceneObject<T>::Create() or SceneObjectLoadByType() instead of the constructor.");
}

Asset::~Asset()
{
}

void Asset::Init()
{
	if (hasBeenInit)
		Logger::Log(LogSeverity::ERROR, "SceneObject is already initialized.");

	hasBeenInit = true;

	ID = AssetManager::GetIDCounter();
	AssetManager::IncrementIDCounter();
}

void Asset::Init(nlohmann::json data)
{
	if (hasBeenInit)
		Logger::Log(LogSeverity::ERROR, "SceneObject is already initialized.");

	Load(data);

	hasBeenInit = true;
}

std::shared_ptr<Asset> Asset::LoadByType(std::string name)
{
	using json = nlohmann::json;

	std::ifstream input(PATH + name + ".json");

	if (input.fail())
	{
		Logger::Log(LogSeverity::ERROR, "Unable to open " + name + ".json asset file");
		return std::shared_ptr<Asset>();
	}

	json data;
	input >> data;

	std::string type = data["Asset"]["ClassType"];

	std::shared_ptr<Asset> returnObject;

	isBeingCreated = true;

	if (type == "Asset")
		Logger::Log(LogSeverity::FATAL_ERROR, "Cant load asset of base type");
	else if (type == "MeshAsset")
		returnObject.reset(new MeshAsset());

	isBeingCreated = false;

	if (returnObject == nullptr)
		Logger::Log(LogSeverity::ERROR, "Asset of type: " + type + " not found");
	else
	{
		json assetData = data["Asset"];
		returnObject->Init(assetData);
	}

	return returnObject;
}

uint64_t Asset::GetID() const
{
	return ID;
}

std::string Asset::GetType()
{
	return "BaseAsset";
}

void Asset::GUI()
{
	if (ImGui::Button("Save"))
	{
		WriteToFile();
	}

	ImGui::Text(std::to_string(ID).c_str());
	ImGui::InputText("Name", &name);
}

void Asset::WriteToFile()
{
	std::ofstream output(PATH + name + ".json");
	output << std::setw(4) << this->Save();
}

nlohmann::json Asset::Save()
{
	using json = nlohmann::json;

	json assetData;

	assetData["Asset"]["ID"] = ID;
	assetData["Asset"]["ClassType"] = GetType();
	assetData["Asset"]["Name"] = name;

	return assetData;
}

void Asset::Load(nlohmann::json data)
{
	using json = nlohmann::json;

	ID = data["ID"];
	name = data["Name"];
}
