#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "VulkanDescriptor.h"
#include "VulkanGraphicPipeline.h"
#include "Texture.h"
#include "VulkanHelper.h"

class Model
{
public:
	Mesh* mesh = nullptr;
	Texture* texture = nullptr;
	VulkanGraphicPipeline* graphicPipeline = nullptr;
	glm::vec3 position; // Move this in a transform class?

private:
	VkDevice device;
	VkPhysicalDevice physicalDevice;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::unique_ptr <VulkanDescriptor> descriptor;

public:
	Model(VkDevice device, VkPhysicalDevice physicalDevice, size_t swapchainImagesSize, Mesh* mesh, Texture* texture, VulkanGraphicPipeline* graphicPipeline);
	~Model();

	void Draw(VkCommandBuffer commandBuffer, int i);
	void UpdateUniformBuffer(uint32_t currentImage, VulkanHelper::UniformBufferObject* ubo);
	void Recreate(size_t swapchainImagesSize);

private:
	void Create(size_t swapchainImagesSize);
	void Cleanup();
};