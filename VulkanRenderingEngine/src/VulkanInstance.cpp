#include "VulkanInstance.h"
#include <iostream>

VkDebugUtilsMessageSeverityFlagBitsEXT VulkanInstance::validationLayerMessageMinSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

const std::vector<const char*> VulkanInstance::VALIDATION_LAYERS =
{
	"VK_LAYER_KHRONOS_validation"
};

VulkanInstance::VulkanInstance(VkDebugUtilsMessageSeverityFlagBitsEXT validationLayerMessageMinSeverity)
{
	VulkanInstance::validationLayerMessageMinSeverity = validationLayerMessageMinSeverity;

	if (ENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	// Application info for optimization by driver no really use full in this engine
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "VulkanEngine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	auto extensions = GetRequiredExtensions();

	// Info for creating our instance
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	if (ENABLE_VALIDATION_LAYERS)
	{
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		PopulateDebugMessengerCreateInfo(debugCreateInfo);

		createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
		createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)& debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}

	SetupDebugMessenger();
}

VulkanInstance::~VulkanInstance()
{
	if (ENABLE_VALIDATION_LAYERS)
	{
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroyInstance(instance, nullptr);

	std::cout << "Vulkan instance destroyed" << std::endl;
}

VkInstance VulkanInstance::GetInstance() const
{
	return instance;
}

std::vector<const char*> VulkanInstance::GetRequiredExtensions() const
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (ENABLE_VALIDATION_LAYERS)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

void VulkanInstance::SetupDebugMessenger()
{
	if (!ENABLE_VALIDATION_LAYERS) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

bool VulkanInstance::CheckValidationLayerSupport() const
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : VALIDATION_LAYERS)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
			return false;
	}

	return true;
}

void VulkanInstance::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

VkResult VulkanInstance::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VulkanInstance::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	if (messageSeverity < validationLayerMessageMinSeverity)
		return VK_FALSE;

	std::string severity = "Nothing";

	switch (messageSeverity)
	{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			severity = "Verbose";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			severity = "Info";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			severity = "Warning";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			severity = "Error";
			break;
	}

	std::string type = "None";

	switch (messageType)
	{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
			type = "General";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
			type = "Validation";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
			type = "Performance";
			break;
	}

	//TODO: Add log print here
	std::cerr << "Validation layer: " << severity << " " << type << " " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}