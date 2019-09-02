#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <glm/glm.hpp>

#include "Rendering/Mesh.h"
#include "Rendering/Vulkan/VulkanDescriptor.h"
#include "Rendering/Vulkan/VulkanGraphicPipeline.h"
#include "Rendering/Texture.h"
#include "Rendering/Vulkan/VulkanHelper.h"

class Model
{
public:
	Mesh* mesh = nullptr;
	Texture* texture = nullptr;
	Texture* normalTexture = nullptr;
	VulkanGraphicPipeline* graphicPipeline = nullptr;
	glm::vec3 position = glm::vec3(0); // Move this in a transform class?
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
	std::string meshName = "Cube";
	std::string textureName = "Debug.jpg";

private:
	VkDevice device;
	VkPhysicalDevice physicalDevice;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::unique_ptr <VulkanDescriptor> descriptor;

public:
	Model(VkDevice device, VkPhysicalDevice physicalDevice, size_t swapchainImagesSize, Mesh* mesh, Texture* texture, Texture* normalTexture, VulkanGraphicPipeline* graphicPipeline);
	~Model();

	void Draw(VkCommandBuffer commandBuffer, int i);
	void UpdateUniformBuffer(uint32_t currentImage, VulkanHelper::UniformBufferObject* ubo);
	void Recreate(size_t swapchainImagesSize);

private:
	void Create(size_t swapchainImagesSize);
	void Cleanup();
};