#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <stdexcept>
#include <memory>

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanGraphicPipeline.h"
#include "VulkanShader.h"
#include "Texture.h"
#include "Mesh.h"
#include "VulkanDescriptor.h"
#include "GlfwManager.h"

class VulkanManager
{
public:
	static const int MAX_FRAMES_IN_FLIGHT = 2;

private:
	GLFWwindow* window;

	std::unique_ptr<VulkanInstance> vulkanInstance;
	VkSurfaceKHR surface = nullptr;
	std::unique_ptr<VulkanPhysicalDevice> physicalDevice;
	std::unique_ptr<VulkanLogicalDevice> logicalDevice;
	std::unique_ptr<VulkanSwapChain> swapChain;
	std::unique_ptr<VulkanRenderPass> renderPass;

	//TODO: Change this so we can have multiple and take ref in mesh
	std::unique_ptr<VulkanShader> vertexShader;
	std::unique_ptr<VulkanShader> fragShader;
	std::unique_ptr <VulkanGraphicPipeline> graphicPipeline;

	//TODO: Move this
	VkCommandPool commandPool;

	std::unique_ptr<Texture> chaletTexture;
	std::unique_ptr<Mesh> chaletMesh;

	std::unique_ptr<VulkanDescriptor> descriptor;

	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

public:
	VulkanManager(GLFWwindow* window, VkSampleCountFlagBits msaaSamples);
	~VulkanManager();

	void RecreateSwapChain();

	void Draw(GlfwManager* window);

	VulkanInstance* GetVulkanInstance();
	VkSurfaceKHR GetVkSurfaceKHR();
	void WaitForIdle();

private:
	void CreateCommandBuffer();// TODO: Not here?
	void CreateSyncObject();// TODO: Not here?
	void UpdateUniformBuffer(uint32_t currentImage);// TODO: Not here?
};