#include "VulkanLogicalDevice.h"

#include "Rendering/Vulkan/VulkanHelper.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanManager.h"

#include <vector>
#include <set>
#include "Helper/Log.h"

VulkanLogicalDevice::VulkanLogicalDevice()
{
	Logger::Log("Creating logicalDevice");
	VkPhysicalDevice physicalDevice = VulkanRenderer::GetInstance()->GetPhysicalDevice()->GetVk();

	VulkanHelper::QueueFamilyIndices indices = VulkanHelper::FindQueueFamilies();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	// Make queue family create info
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// device feature to enable
	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(VulkanPhysicalDevice::DEVICE_EXTENSIONS.size());
	createInfo.ppEnabledExtensionNames = VulkanPhysicalDevice::DEVICE_EXTENSIONS.data();

	if (VulkanInstance::enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanInstance::VALIDATION_LAYERS.size());
		createInfo.ppEnabledLayerNames = VulkanInstance::VALIDATION_LAYERS.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
	{
		Logger::Log(LogSeverity::FATAL_ERROR, "failed to create logical device!");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

VulkanLogicalDevice::~VulkanLogicalDevice()
{
	vkDestroyDevice(device, nullptr);

	Logger::Log("Logical device destroyed");
}

VkDevice VulkanLogicalDevice::GetVk() const
{
	return device;
}

VkQueue VulkanLogicalDevice::GetGraphicsQueue() const
{
	return graphicsQueue;
}

VkQueue VulkanLogicalDevice::GetPresentQueue() const
{
	return presentQueue;
}
