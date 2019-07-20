#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <vector>
#include <stdexcept>

class VulkanPhysicalDevice
{
public:
	static const std::vector<const char*> DEVICE_EXTENSIONS;

private:
	VkPhysicalDevice physicalDevice = nullptr;
	VkSurfaceKHR surface = nullptr;

	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

public:
	VulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkSampleCountFlagBits msaaSamples);

	VkSampleCountFlagBits GetMsaaSample() const;
	VkSampleCountFlagBits GetMaxUsableSampleCount() const;
	VkPhysicalDevice GetVKPhysicalDevice() const;

private:
	bool IsDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;
};