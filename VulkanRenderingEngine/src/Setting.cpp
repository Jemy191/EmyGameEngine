#include "Setting.h"
#include "Log.h"

nlohmann::json Setting::data;

void Setting::Add(std::string name, nlohmann::json input)
{
	data[name] = input;
}

nlohmann::json Setting::Get(std::string name, nlohmann::json defaultSetting)
{
	if (data.empty())
		return defaultSetting;

	return data[name];
}

void Setting::Save()
{
	std::ofstream output("Settings/Setting.json");
	output << std::setw(4) << data;
}

void Setting::Load()
{
	using json = nlohmann::json;

	std::ifstream input("Settings/Setting.json");

	if (input.fail())
		return;

	input >> data;
}

Setting::Setting()
{
}

Setting::~Setting()
{
}