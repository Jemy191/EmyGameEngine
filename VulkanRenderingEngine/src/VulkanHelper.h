#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <optional>
#include <vector>
#include <array>
namespace VulkanHelper
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool IsComplete() const;
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities = {};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct CreateTextureParameter
	{
		VkDevice device;
		VkPhysicalDevice physicalDevice;
		VkQueue graphicsQueue;
		VkCommandPool commandPool;

		VkExtent2D extent;
		uint32_t mipLevels;
		VkSampleCountFlagBits msaaSample;

		VkFormat imageFormat;
		VkImageTiling tiling;

		VkImageUsageFlags usage;
		VkMemoryPropertyFlags properties;
		VkImageAspectFlags aspectFlags;

		VkImageLayout oldLayout;
		VkImageLayout newLayout;
	};


	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void CreateImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	//TODO: Shorten parameter list
	void CreateTexture(CreateTextureParameter& parameter, VkImage& image, VkImageView& imageView, VkDeviceMemory& imageMemory);
	void TransitionImageLayout(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

	void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void CopyBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue graphicQueue, VkBuffer buffer, VkImage image, VkExtent2D extent);

	void GenerateMipmaps(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicQueue, VkImage image, VkFormat imageFormat, VkExtent2D extent, uint32_t mipLevels);


	void CopyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	//TODO: better handling of these. Make them not one time but go in a bigger CommandBuffer
	VkCommandBuffer BeginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
	void EndSingleTimeCommands(VkDevice device, VkQueue graphicsQueue, VkCommandBuffer commandBuffer, VkCommandPool commandPool);
	bool HasStencilComponent(VkFormat format);

	VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);
	VkFormat FindSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec2 texCoord;
		glm::vec3 tangent;
		glm::vec3 biTangent;

		bool operator==(const Vertex& other) const;

		static VkVertexInputBindingDescription GetBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 6> GetAttributeDescriptions();
	};

	struct UniformBufferObject
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::vec3 viewPos;
	};
}

namespace std
{
	template<> struct hash<VulkanHelper::Vertex>
	{
		size_t operator()(VulkanHelper::Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}