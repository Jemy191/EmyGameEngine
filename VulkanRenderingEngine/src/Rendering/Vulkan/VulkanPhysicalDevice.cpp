#include "VulkanPhysicalDevice.h"
#include <algorithm>
#include <set>
#include <string>
#include "Helper/Log.h"
#include "Rendering/Vulkan/VulkanHelper.h"
#include "VulkanManager.h"

const std::vector<const char*> VulkanPhysicalDevice::DEVICE_EXTENSIONS =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VulkanPhysicalDevice::VulkanPhysicalDevice(VkSampleCountFlagBits msaaSamples)
{
	Logger::Log("Creating physicalDevice");
	VkInstance instance = VulkanManager::GetInstance()->GetVulkanInstance()->GetInstance();

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		Logger::Log(LogSeverity::FATAL_ERROR, "failed to find GPUs with Vulkan support!");
	}

	// Try to find a suitable device
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			physicalDevice = device;

			VkSampleCountFlagBits maxMsaaSample = GetMaxUsableSampleCount();

			if (msaaSamples <= maxMsaaSample)
				this->msaaSamples = msaaSamples;
			else
				this->msaaSamples = maxMsaaSample;

			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		Logger::Log(LogSeverity::FATAL_ERROR, "failed to find a suitable GPU!");
	}
}

VkSampleCountFlagBits VulkanPhysicalDevice::GetMaxUsableSampleCount() const
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

	VkSampleCountFlags counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);

	if (counts & VK_SAMPLE_COUNT_64_BIT)
		return VK_SAMPLE_COUNT_64_BIT;
	if (counts & VK_SAMPLE_COUNT_32_BIT)
		return VK_SAMPLE_COUNT_32_BIT;
	if (counts & VK_SAMPLE_COUNT_16_BIT)
		return VK_SAMPLE_COUNT_16_BIT;
	if (counts & VK_SAMPLE_COUNT_8_BIT)
		return VK_SAMPLE_COUNT_8_BIT;
	if (counts & VK_SAMPLE_COUNT_4_BIT)
		return VK_SAMPLE_COUNT_4_BIT;
	if (counts & VK_SAMPLE_COUNT_2_BIT)
		return VK_SAMPLE_COUNT_2_BIT;

	return VK_SAMPLE_COUNT_1_BIT;
}

VkPhysicalDevice VulkanPhysicalDevice::GetVKPhysicalDevice() const
{
	return physicalDevice;
}

VkSampleCountFlagBits VulkanPhysicalDevice::GetMsaaSample() const
{
	return msaaSamples;
}

bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device)
{
	VulkanHelper::QueueFamilyIndices indices = VulkanHelper::FindQueueFamilies(device);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		VulkanHelper::SwapChainSupportDetails swapChainSupport = VulkanHelper::QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy && supportedFeatures.fillModeNonSolid;
}

bool VulkanPhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device) const
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool VulkanHelper::QueueFamilyIndices::IsComplete() const
{
	return graphicsFamily.has_value() && presentFamily.has_value();
}
