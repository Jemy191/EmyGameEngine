#include "VulkanShader.h"

#include "Log.h"
#include <fstream>
#include <memory>

const std::string VulkanShader::PATH = "Assets/Shaders/";

VulkanShader::VulkanShader(VkDevice device, std::string name, VkShaderStageFlagBits stage, const char* entryPointName)
{
	this->device = device;

	auto code = LoadShader(name + ".spv");

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		Logger::Log(LogSeverity::FATAL_ERROR, "failed to create shader module!");
	}

	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = stage;
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = entryPointName;

	Logger::Log("Shader created");
}

VulkanShader::~VulkanShader()
{
	//TODO: Move this somewhere else
	vkDestroyShaderModule(device, shaderModule, nullptr);

	Logger::Log("Shader destroyed");
}

VkPipelineShaderStageCreateInfo VulkanShader::GetShaderStageInfo() const
{
	return shaderStageInfo;
}

VkShaderModule VulkanShader::GetVkShaderModule() const
{
	return shaderModule;
}

std::vector<char> VulkanShader::LoadShader(std::string fileName)
{
	std::ifstream file(PATH + fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		Logger::Log(LogSeverity::FATAL_ERROR, "failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}
