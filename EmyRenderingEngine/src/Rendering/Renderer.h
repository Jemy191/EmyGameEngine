#pragma once
#include "Game/Setting.h"
#include <json.hpp>

enum class GraphicalAPI : int
{
	OPENGL = 0,
	VULKAN = 1,
	METAL = 2,
	DIRECTX12 = 3
};

NLOHMANN_JSON_SERIALIZE_ENUM(GraphicalAPI, {
	{GraphicalAPI::OPENGL, "OPENGL"},
	{GraphicalAPI::VULKAN, "VULKAN"},
	{GraphicalAPI::METAL, "METAL"},
	{GraphicalAPI::DIRECTX12, "DIRECTX12"},
	})


class Renderer
{
public:
	static bool IsGraphicalAPI(GraphicalAPI api) { return Setting::Get("GraphicAPI", GraphicalAPI::OPENGL) == api; }
};