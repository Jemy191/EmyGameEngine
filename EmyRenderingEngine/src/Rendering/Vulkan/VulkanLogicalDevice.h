#pragma once
#include "Header/GLFWHeader.h"

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