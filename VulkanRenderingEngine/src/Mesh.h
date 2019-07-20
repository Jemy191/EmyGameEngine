#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanHelper.h"

#include <glm/glm.hpp>
#include <string>

class Mesh
{
private:
	static const std::string PATH;

	VkDevice device;

	std::vector<VulkanHelper::Vertex> vertices;
	std::vector<uint32_t> indices;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

public:
	Mesh(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicQueue, std::string meshName);
	~Mesh();

	void CmdBind(VkCommandBuffer commandBuffer);
	void CmdDraw(VkCommandBuffer commandBuffer);

private:

};