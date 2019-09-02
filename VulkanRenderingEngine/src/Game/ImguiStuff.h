#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Imgui/imgui.h"
#include "Imgui/imgui_stdlib.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_vulkan.h"

class ImguiStuff
{
public:
	ImguiStuff(GLFWwindow* window, uint32_t queueFamily, VkCommandPool globalCommandPool);
	~ImguiStuff();

	void StartFrame();
	void EndFrame();

	void Draw(VkCommandBuffer commandBuffer);

private:
	VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
	VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
};