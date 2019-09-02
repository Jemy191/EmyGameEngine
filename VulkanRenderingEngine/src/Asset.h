#pragma once
#include <string>
#include <json.hpp>
#include "Log.h"
#include "ImguiStuff.h"
#include <memory>

class Asset
{
	static bool isBeingCreated;
	static bool hasBeenInit;
	static const std::string PATH;
	uint64_t ID = 0;

public:
	std::string name = "Unnamed";

	virtual void Init();
	virtual void Init(nlohmann::json data);
	virtual ~Asset();

	template<class T>
	static std::shared_ptr<Asset> Create();
	static std::shared_ptr<Asset> LoadByType(std::string name);

	uint64_t GetID() const;
	virtual std::string GetType();
	virtual void GUI();

	void WriteToFile();

protected:
	Asset();
	virtual nlohmann::json Save();
	virtual void Load(nlohmann::json data);

};

template<class T>
std::shared_ptr<Asset> Asset::Create()
{
	isBeingCreated = true;
	std::shared_ptr<Asset> object(new T());
	isBeingCreated = false;

	object->Init();
	return object;
}