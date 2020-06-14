#pragma once
#include "Game/Setting.h"
#include <json.hpp>
#include <memory>
#include "Header/GLFWHeader.h"
#include "Rendering/UI/ImguiBase.h"
#include <Rendering\GlfwManager.h>

enum class GraphicalAPI : int
{
	OPENGL = 0,
	VULKAN = 1,
	METAL = 2,
	DIRECTX12 = 3
};

NLOHMANN_JSON_SERIALIZE_ENUM(GraphicalAPI,
	{
	{GraphicalAPI::OPENGL, "OPENGL"},
	{GraphicalAPI::VULKAN, "VULKAN"},
	{GraphicalAPI::METAL, "METAL"},
	{GraphicalAPI::DIRECTX12, "DIRECTX12"},
	})


class Renderer
{
protected:
	std::unique_ptr<ImguiBase> imgui;

public:
	static bool IsGraphicalAPI(GraphicalAPI api);
	static std::unique_ptr<Renderer> CreateGraphicalAPI(GraphicalAPI api, GLFWwindow* window);

	Renderer(GLFWwindow* window);

	virtual void Present(GlfwManager* window) {};
	virtual void WaitForIdle() {}

	ImguiBase* GetImgui() const;
};