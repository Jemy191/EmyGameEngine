#include "VulkanSwapChain.h"

#include "Rendering/Vulkan/VulkanHelper.h"

#include <algorithm>
#include "Helper/Log.h"
#include "VulkanManager.h"

VulkanSwapChain::VulkanSwapChain(GLFWwindow* window)
{
	Logger::Log("Creating swapChain");

	VkDevice device = VulkanManager::GetInstance()->GetLogicalDevice()->GetVk();
	VkPhysicalDevice physicalDevice = VulkanManager::GetInstance()->GetPhysicalDevice()->GetVk();
	VkSurfaceKHR surface = VulkanManager::GetInstance()->GetVkSurfaceKHR();
	VkSampleCountFlagBits msaaSample = VulkanManager::GetInstance()->GetPhysicalDevice()->GetMsaaSample();
	VkQueue graphicQueue = VulkanManager::GetInstance()->GetLogicalDevice()->GetGraphicsQueue();
	VkRenderPass renderPass = VulkanManager::GetInstance()->GetRenderPass()->GetVk();

	VulkanHelper::SwapChainSupportDetails swapChainSupport = VulkanHelper::QuerySwapChainSupport();

	VkSurfaceFormatKHR surfaceFormat = VulkanHelper::ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, window);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	VulkanHelper::QueueFamilyIndices indices = VulkanHelper::FindQueueFamilies();
	uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		Logger::Log(LogSeverity::FATAL_ERROR, "failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	swapChainImageViews.resize(swapChainImages.size());

	for (uint32_t i = 0; i < swapChainImages.size(); i++)
	{
		swapChainImageViews[i] = VulkanHelper::CreateImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}

	// FrameBuffer
	VulkanHelper::CreateTextureParameter colorTextureParameter = {};
	colorTextureParameter.extent = extent;
	colorTextureParameter.mipLevels = 1;
	colorTextureParameter.msaaSample = msaaSample;
	colorTextureParameter.imageFormat = swapChainImageFormat;
	colorTextureParameter.tiling = VK_IMAGE_TILING_OPTIMAL;
	colorTextureParameter.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	colorTextureParameter.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	colorTextureParameter.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	colorTextureParameter.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorTextureParameter.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VulkanHelper::CreateTexture(colorTextureParameter, colorImage, colorImageView, colorImageMemory);


	VkFormat depthFormat = VulkanHelper::FindDepthFormat();
	VulkanHelper::CreateTextureParameter depthTextureParameter = {};
	depthTextureParameter.extent = extent;
	depthTextureParameter.mipLevels = 1;
	depthTextureParameter.msaaSample = msaaSample;
	depthTextureParameter.imageFormat = depthFormat;
	depthTextureParameter.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthTextureParameter.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depthTextureParameter.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	depthTextureParameter.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
	depthTextureParameter.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthTextureParameter.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VulkanHelper::CreateTexture(depthTextureParameter, depthImage, depthImageView, depthImageMemory);

	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		std::array<VkImageView, 3> attachments = {
			colorImageView,
			depthImageView,
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
		{
			Logger::Log(LogSeverity::FATAL_ERROR, "failed to create framebuffer!");
		}
	}
}

VulkanSwapChain::~VulkanSwapChain()
{
	VkDevice device = VulkanManager::GetInstance()->GetLogicalDevice()->GetVk();
	vkDestroyImageView(device, colorImageView, nullptr);
	vkDestroyImage(device, colorImage, nullptr);
	vkFreeMemory(device, colorImageMemory, nullptr);

	vkDestroyImageView(device, depthImageView, nullptr);
	vkDestroyImage(device, depthImage, nullptr);
	vkFreeMemory(device, depthImageMemory, nullptr);

	for (auto framebuffer : swapChainFramebuffers)
	{
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	for (auto imageView : swapChainImageViews)
	{
		vkDestroyImageView(device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(device, swapChain, nullptr);

	Logger::Log("Swap chain destroyed");
}

VkSwapchainKHR VulkanSwapChain::GetVkSwapchainKHR() const
{
	return swapChain;
}

std::vector<VkImage> VulkanSwapChain::GetVkImages() const
{
	return swapChainImages;
}

std::vector<VkImageView> VulkanSwapChain::GetVkImageViews() const
{
	return swapChainImageViews;
}

std::vector<VkFramebuffer> VulkanSwapChain::GetSwapChainFramebuffers() const
{
	return swapChainFramebuffers;
}

VkFormat VulkanSwapChain::GetSwapChainImageFormat() const
{
	return swapChainImageFormat;
}

VkExtent2D VulkanSwapChain::GetVkExtent2D() const
{
	return swapChainExtent;
}

VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	/*for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			return availablePresentMode;
	}*/

	return VK_PRESENT_MODE_FIFO_KHR; // use for vsync
}

VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) const
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}