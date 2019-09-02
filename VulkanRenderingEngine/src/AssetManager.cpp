#include "AssetManager.h"
#include "Log.h"
#include "ImguiStuff.h"
#include "MeshAsset.h"
#include <filesystem>

uint64_t AssetManager::IDCounter = 0;

std::unordered_map<uint64_t, std::shared_ptr<Asset>> AssetManager::assetList;

bool AssetManager::assetManagerWindowOpen = true;

std::string AssetManager::assetToLoad;

uint64_t AssetManager::GetIDCounter()
{
	return IDCounter;
}

void AssetManager::IncrementIDCounter()
{
	IDCounter++;
}

std::shared_ptr<Asset> AssetManager::Get(uint64_t ID)
{
	return assetList[ID];
}

void AssetManager::Register(std::shared_ptr<Asset> asset)
{
	assetList.emplace(asset->GetID(), asset);
}

void AssetManager::GUI()
{
	if (assetManagerWindowOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Asset manager", &assetManagerWindowOpen))
		{
			ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

			ImGui::Text(("Asset count: " + std::to_string(assetList.size())).c_str());

			ImGui::InputText("", &assetToLoad);
			ImGui::SameLine();
			if (std::filesystem::exists("Assets/TempAssetFolder/" + assetToLoad + ".json"))
			{
				if (ImGui::Button("Load"))
				{
					if (auto loadingAsset = Asset::LoadByType(assetToLoad))
					{
						Register(loadingAsset);
					}
				}
			}
			else
				ImGui::Text("File don't exist");

			if (ImGui::Button("Create MeshAsset"))
				Register(Asset::Create<MeshAsset>());

			for (auto assetPair : assetList)
			{
				Asset* asset = assetPair.second.get();
				if (ImGui::TreeNode((asset->name + "|" + asset->GetType() + "###" + std::to_string(asset->GetID())).c_str()))
				{
					asset->GUI();

					ImGui::TreePop();
				}
			}
		}
		ImGui::End();
	}
}

AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{
}