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
	VulkanLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	~VulkanLogicalDevice();

	VkDevice GetVKDevice() const;
	VkQueue GetGraphicsQueue() const;
	VkQueue GetPresentQueue() const;
};