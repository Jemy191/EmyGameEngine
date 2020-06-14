#pragma once
#include "Header/GLFWHeader.h"

#include <optional>
#include <vector>

class VulkanPhysicalDevice
{
public:
	static const std::vector<const char*> DEVICE_EXTENSIONS;

private:
	VkPhysicalDevice physicalDevice = nullptr;

	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

public:
	VulkanPhysicalDevice(VkSampleCountFlagBits msaaSamples);

	VkSampleCountFlagBits GetMsaaSample() const;
	VkSampleCountFlagBits GetMaxUsableSampleCount() const;
	VkPhysicalDevice GetVk() const;

private:
	bool IsDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;
};