#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class VulkanSwapChain
{
private:
	VkSwapchainKHR swapChain = nullptr;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	VkImage colorImage = nullptr;
	VkDeviceMemory colorImageMemory = nullptr;
	VkImageView colorImageView = nullptr;
	VkImage depthImage = nullptr;
	VkDeviceMemory depthImageMemory = nullptr;
	VkImageView depthImageView = nullptr;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	VkDevice device = nullptr;

public:
	VulkanSwapChain(GLFWwindow* window, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSampleCountFlagBits msaaSample, VkCommandPool commandPool, VkQueue graphicQueue, VkRenderPass renderPass);
	~VulkanSwapChain();

	VkSwapchainKHR GetVkSwapchainKHR() const;
	std::vector<VkImage> GetVkImages() const;
	std::vector<VkBuffer> GetUniformBuffers() const;
	std::vector<VkImageView> GetVkImageViews() const;
	std::vector<VkFramebuffer> GetSwapChainFramebuffers() const;
	std::vector<VkDeviceMemory> GetUniformBuffersMemory() const;
	VkFormat GetSwapChainImageFormat() const;
	VkExtent2D GetVkExtent2D() const;
private:
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) const;
};