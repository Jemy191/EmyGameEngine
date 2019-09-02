#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class VulkanLayoutBinding
{
private:
	VkDevice device = nullptr;

	VkDescriptorSetLayout descriptorSetLayout = {};
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

public:
	void AddLayoutBinding(VkDescriptorType descriptorType, VkShaderStageFlags stageFlags);
	void Create(VkDevice device);
	~VulkanLayoutBinding();

	VkDescriptorSetLayout GetVkDescriptorSetLayout() const;
};