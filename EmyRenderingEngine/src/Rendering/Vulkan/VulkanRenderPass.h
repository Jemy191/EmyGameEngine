#pragma once
#include "Header/GLFWHeader.h"

#include <vector>

class VulkanRenderPass
{
private:
	VkRenderPass renderPass = nullptr;

public:
	VulkanRenderPass();
	~VulkanRenderPass();

	VkRenderPass GetVk() const;
};