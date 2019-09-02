#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

class VulkanShader
{
private:
	static const std::string PATH;

	VkPipelineShaderStageCreateInfo shaderStageInfo = {};
	VkShaderModule shaderModule = nullptr;

public:
	VulkanShader(std::string name, VkShaderStageFlagBits stage, const char* entryPointName = "main");
	~VulkanShader();

	VkPipelineShaderStageCreateInfo GetShaderStageInfo() const;
	VkShaderModule GetVkShaderModule() const;

private:
	static std::vector<char> LoadShader(std::string fileName);
};