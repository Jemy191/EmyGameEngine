#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanLayoutBinding.h"
#include "VulkanShader.h"

#include <unordered_map>
#include <string>
#include <vector>

class VulkanGraphicPipeline
{
public:
	VulkanLayoutBinding layoutBinding;

private:
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

	VkDevice device = nullptr;

	VkPipelineLayout pipelineLayout = nullptr;
	VkPipeline graphicsPipeline = nullptr;

public:
	void Create(VkDevice device, VkExtent2D swapChainExtent, VkRenderPass renderPass, VkSampleCountFlagBits msaaSamples, VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL);
	~VulkanGraphicPipeline();

	/// <summary>
	/// Add a shader stage. There can only be one shader stage of each stage type;
	/// </summary>
	/// <param name="shaderStage">The shader</param>
	/// <param name="replace">Do we replace the existing shader stage if there one</param>
	/// <returns>Return false if there already a shader state of the type provided unless replace is enable</returns>
	bool AddShader(VulkanShader* shader, bool replace = false);

	VkPipelineLayout GetVkPipelineLayout() const;
	VkPipeline GetVkPipeline() const;
};