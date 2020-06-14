#pragma once
#include "Header/GLFWHeader.h"

#include "Header/ImguiHeader.h"
#include "Imgui/imgui_impl_vulkan.h"
#include "Rendering/UI/ImguiBase.h"

class ImguiVulkan : public ImguiBase
{
public:
	ImguiVulkan(GLFWwindow* window, uint32_t queueFamily);
	~ImguiVulkan();

	void StartFrame() override;

	void Draw(VkCommandBuffer commandBuffer);

private:
	VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
	VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
};