#pragma once
#include "Asset.h"
#include <unordered_map>
#include <memory>

class AssetManager
{
private:
	static uint64_t IDCounter;

	static std::unordered_map<uint64_t, std::shared_ptr<Asset>> assetList;

	static bool assetManagerWindowOpen;

	static std::string assetToLoad;

public:
	static void Init();

	static uint64_t GetIDCounter();
	static void IncrementIDCounter();

	static std::shared_ptr<Asset> Get(uint64_t ID);
	static void Register(std::shared_ptr<Asset> asset);

	static void GUI();

private:
	AssetManager();
	~AssetManager();
};
