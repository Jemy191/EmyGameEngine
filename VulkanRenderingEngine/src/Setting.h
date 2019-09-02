#pragma once
#include <fstream>
#include <json.hpp>
#include <iomanip>

class Setting
{
private:
	static nlohmann::json data;

public:
	static void Add(std::string name, nlohmann::json input);
	static nlohmann::json Get(std::string name);
	static void Save();
	static void Load();

private:
	Setting();
	~Setting();
};