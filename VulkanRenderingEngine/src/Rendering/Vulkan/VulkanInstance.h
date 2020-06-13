#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class VulkanInstance
{
public:
	static bool enableValidationLayers;
	static const std::vector<const char*> VALIDATION_LAYERS;

private:
	VkInstance instance;

	VkDebugUtilsMessengerEXT debugMessenger;
	static VkDebugUtilsMessageSeverityFlagBitsEXT validationLayerMessageMinSeverity;

public:
	VulkanInstance(VkDebugUtilsMessageSeverityFlagBitsEXT validationLayerMessageMinSeverity);
	~VulkanInstance();

	VkInstance GetVk() const;

private:
	std::vector<const char*> GetRequiredExtensions() const;

	//Validation Layer Stuff
	void SetupDebugMessenger();
	bool CheckValidationLayerSupport() const;
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};