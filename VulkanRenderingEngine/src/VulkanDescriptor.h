#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include "Texture.h"

// En gros on dirais que c'est les parametre du shader
class VulkanDescriptor
{
private:
	VkDevice device = nullptr;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

public:
	VulkanDescriptor(VkDevice device, size_t swapchainImageCount, std::vector<VkBuffer> uniformBuffers, VkDescriptorSetLayout descriptorSetLayout, Texture* texture);
	~VulkanDescriptor();

	void CmdBind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int i);
};