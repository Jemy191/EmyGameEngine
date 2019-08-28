#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Log.h"
#include <cmath>
#include <algorithm>
#include "Log.h"

#include "VulkanHelper.h"

const std::string Texture::PATH = "Assets/Textures/";

Texture::Texture(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicQueue, std::string name, bool createMipMap)
{
	this->device = device;

	std::string filename = PATH + name;

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (createMipMap)
		mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	if (!pixels)
	{
		Logger::Log(LogSeverity::FATAL_ERROR, "failed to load: "+ filename +" texture image!");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanHelper::CreateBuffer(device, physicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	// Move texture data to the GPU
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device, stagingBufferMemory);

	//Maybe we can keep the data?

	VkExtent2D extent =
	{
		static_cast<uint32_t>(texWidth),
		static_cast<uint32_t>(texHeight)
	};
	
	VulkanHelper::CreateTextureParameter textureParameter = {};
	textureParameter.device = device;
	textureParameter.physicalDevice = physicalDevice;
	textureParameter.commandPool = commandPool;
	textureParameter.extent = extent;
	textureParameter.mipLevels = mipLevels;
	textureParameter.msaaSample = VK_SAMPLE_COUNT_1_BIT;
	textureParameter.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
	textureParameter.tiling = VK_IMAGE_TILING_OPTIMAL;
	textureParameter.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	textureParameter.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	textureParameter.graphicsQueue = graphicQueue;
	textureParameter.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	textureParameter.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	textureParameter.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	VulkanHelper::CreateTexture(textureParameter, textureImage, textureImageView, textureImageMemory);

	VulkanHelper::CopyBufferToImage(device, commandPool, graphicQueue, stagingBuffer, textureImage, extent);
	//transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

	VulkanHelper::GenerateMipmaps(device, physicalDevice, commandPool, graphicQueue, textureImage, VK_FORMAT_R8G8B8A8_UNORM, extent, mipLevels);

	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.minLod = 0;
	samplerInfo.maxLod = static_cast<float>(mipLevels);
	samplerInfo.mipLodBias = 0;

	if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
	{
		Logger::Log(LogSeverity::FATAL_ERROR, "failed to create texture sampler!");
	}
}

Texture::~Texture()
{
	vkDestroyImage(device, textureImage, nullptr);
	vkFreeMemory(device, textureImageMemory, nullptr);
	vkDestroyImageView(device, textureImageView, nullptr);
	vkDestroySampler(device, textureSampler, nullptr);

	Logger::Log("Texture destroyed");
}

bool Texture::GetHasAlpha() const
{
	return hasAlpha;
}

uint32_t Texture::GetMipLevels() const
{
	return mipLevels;
}

VkImage Texture::GetTextureImage() const
{
	return textureImage;
}

VkImageView Texture::GetTextureImageView() const
{
	return textureImageView;
}

VkSampler Texture::GetTextureSampler() const
{
	return textureSampler;
}
