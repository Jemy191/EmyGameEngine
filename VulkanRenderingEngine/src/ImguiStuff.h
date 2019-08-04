#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_vulkan.h"

class ImguiStuff
{
public:
	ImguiStuff(VkDevice device, GLFWwindow* window, VkInstance instance, VkPhysicalDevice physicalDevice, VkRenderPass renderPass, uint32_t queueFamily, VkQueue queue, VkCommandPool commandPool, uint32_t imageCount, uint32_t minImageCount);
	~ImguiStuff();

	void StartFrame();
	void EndFrame();

	void Draw(VkCommandBuffer commandBuffer);

private:
	VkDevice device;

	VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
	VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
};