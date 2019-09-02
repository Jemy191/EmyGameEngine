#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class VulkanRenderPass
{
private:
	VkRenderPass renderPass = nullptr;

	VkDevice device = nullptr;

public:
	VulkanRenderPass(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSampleCountFlagBits msaaSamples);
	~VulkanRenderPass();

	VkRenderPass GetVkRenderPass() const;
};