#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VulkanLogicalDevice
{
private:
	VkDevice device = nullptr;
	VkQueue graphicsQueue = nullptr;
	VkQueue presentQueue = nullptr;

public:
	VulkanLogicalDevice();
	~VulkanLogicalDevice();

	VkDevice GetVk() const;
	VkQueue GetGraphicsQueue() const;
	VkQueue GetPresentQueue() const;
};