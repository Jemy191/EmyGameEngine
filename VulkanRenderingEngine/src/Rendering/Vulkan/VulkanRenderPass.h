#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class VulkanRenderPass
{
private:
	VkRenderPass renderPass = nullptr;

public:
	VulkanRenderPass();
	~VulkanRenderPass();

	VkRenderPass GetVkRenderPass() const;
};